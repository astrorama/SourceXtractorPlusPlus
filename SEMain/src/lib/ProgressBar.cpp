#include "SEMain/ProgressBar.h"
#include "ModelFitting/ModelFitting/utils.h"

#include <sys/ioctl.h>
#include <iostream>
#include <boost/date_time.hpp>
#include <signal.h>
#include <boost/thread.hpp>
#include <curses.h>
#include <term.h>

namespace SExtractor {

static struct sigaction sigterm, sigwinch;
static std::map<int, struct sigaction> prev_signal;

// Forward declaration
static void handleTerminatingSignal(int s);
static void handleTerminalResize(int s);

// Terminal capabilities
static const std::string ENTER_CUP{"smcup"};
static const std::string EXIT_CUP{"rmcup"};
static const std::string SHOW_CURSOR{"cvvis"};
static const std::string HIDE_CURSOR{"civis"};
static const std::string CLEAR_SCREEN{"clear"};
static const std::string SAVE_CURSOR{"sc"};
static const std::string RESTORE_CURSOR{"rc"};
static const std::string CURSOR_POSITION{"cup"};
static const std::string CLEAR_LINE{"el"};
static const std::string COLUMN{"hpa"};
static const std::string RESTORE_COLORS{"op"};
static const std::string RESTORE_STYLE{"sgr0"};
static const std::string BOLD{"bold"};
static const std::string SET_BACKGROUND{"setab"};
static const std::string SET_FOREGROUND{"setaf"};
static const std::string CHANGE_SCROLLING{"csr"};
static const std::string SCROLL_FORWARD{"ind"};
static int COLOR_BRIGHT = 010;

/**
 * Return the capability string corresponding to the given code
 */
template<typename ...Args>
static std::string capability(const std::string& cap, Args... args) {
  char *fmt = tigetstr(const_cast<char *>(cap.c_str()));
  if (!fmt) {
    return "";
  }
  return tparm(fmt, std::forward<Args>(args)...);
}

/**
 * Wrap the terminal. There is one single logical instance, so everything is static.
 * This used to be inside ProgressBar, but it turns out that the destructor was not being called
 * when unwinding the stack. (Probably because it was a singleton?)
 * ProgressBar can *not* be a singleton if we isolate this data and code here.
 */
struct Terminal {
  struct winsize m_winsize;
  int m_infolines, m_progress_row, m_value_position;
  int m_bar_width;

  void restore() {
    // Restore full scroll and  attributes
    std::cerr << capability(CHANGE_SCROLLING, 0, m_winsize.ws_row)
              << capability(RESTORE_STYLE) << capability(SHOW_CURSOR)
              << capability(EXIT_CUP);
    std::cerr.flush();

    // Restore signal handlers
    ::sigaction(SIGINT, &prev_signal[SIGINT], nullptr);
    ::sigaction(SIGTERM, &prev_signal[SIGTERM], nullptr);
    ::sigaction(SIGABRT, &prev_signal[SIGABRT], nullptr);
    ::sigaction(SIGSEGV, &prev_signal[SIGSEGV], nullptr);
    ::sigaction(SIGHUP, &prev_signal[SIGHUP], nullptr);
    ::sigaction(SIGWINCH, nullptr, nullptr);
  }

  void prepare(bool resize) {
    // C++ guarantees that a static object is initialized once, and in a thread-safe manner
    // We do it here so this setup only happens *if* the progress bar is used
    struct Helper {
      Helper() {
        ::memset(&sigterm, 0, sizeof(sigterm));

        sigterm.sa_handler = &handleTerminatingSignal;
        ::sigaction(SIGINT, &sigterm, &prev_signal[SIGINT]);
        ::sigaction(SIGTERM, &sigterm, &prev_signal[SIGTERM]);
        ::sigaction(SIGABRT, &sigterm, &prev_signal[SIGABRT]);
        ::sigaction(SIGSEGV, &sigterm, &prev_signal[SIGSEGV]);
        ::sigaction(SIGHUP, &sigterm, &prev_signal[SIGHUP]);

        sigwinch.sa_handler = &handleTerminalResize;
        ::sigaction(SIGWINCH, &sigwinch, nullptr);

        // Swap screen buffer
        std::cerr << capability(ENTER_CUP);
      }
    } _helper;

    // Reserve the bottom side for the progress report
    ioctl(STDERR_FILENO, TIOCGWINSZ, &m_winsize);

    if (resize) {
      std::cerr << capability(CLEAR_SCREEN);
    }

    m_progress_row = m_winsize.ws_row - m_infolines - 1;
    // Scroll up so lines are preserved
    std::cerr << capability(SCROLL_FORWARD, m_infolines);
    // Block scrolling
    std::cerr << capability(CHANGE_SCROLLING, 0, m_progress_row - 1);

    // Precompute the width of the bars (remember brackets!)
    m_bar_width = m_winsize.ws_col - m_value_position - 2;
  }
};

// Declare static members
static Terminal terminal;

/**
 * Intercept several terminating signals so the terminal style can be restored
 */
static void handleTerminatingSignal(int s) {
  terminal.restore();

  // Call the previous handler
  ::sigaction(s, &prev_signal[s], nullptr);
  ::raise(s);
}

/**
 * Intercept terminal window resize
 */
static void handleTerminalResize(int) {
  terminal.prepare(true);
}


ProgressBar::ProgressBar()
  : m_started{boost::posix_time::second_clock::local_time()} {
  terminal.prepare(false);
}

ProgressBar::~ProgressBar() {
  terminal.restore();
}

bool ProgressBar::isTerminalCapable() {
  setupterm(nullptr, fileno(stderr), nullptr);
  return isatty(fileno(stderr)) && tigetstr(const_cast<char *>(ENTER_CUP.c_str())) != nullptr;
}

void ProgressBar::handleMessage(const std::map<std::string, std::pair<int, int>>& info) {
  this->ProgressReporter::handleMessage(info);

  // On first call, prepare and spawn the progress report block
  if (!m_progress_thread) {
    // Put always the value at the same distance: length of the longest attribute + space + (space|[) (starts at 1!)
    size_t max_attr_len = 0;

    for (auto& e : info) {
      max_attr_len = std::max(max_attr_len, e.first.size());
    }

    terminal.m_value_position = max_attr_len + 3;
    terminal.m_infolines = info.size();

    // Prepare the scrolling area
    terminal.prepare(false);

    // Start printer
    m_progress_thread = make_unique<boost::thread>(ProgressBar::printThread, this);
  }
}

void ProgressBar::handleMessage(const bool& done) {
  this->ProgressReporter::handleMessage(done);
  if (m_progress_thread)
    m_progress_thread->join();
  terminal.restore();
}

void ProgressBar::printThread(void *d) {
  auto self = static_cast<ProgressBar*>(d);
  while (!self->m_done) {
    auto now = boost::posix_time::second_clock::local_time();
    auto elapsed = now - self->m_started;

    std::ostringstream buffer;

    // Store position and attributes, hide cursor
    buffer << capability(SAVE_CURSOR) << capability(HIDE_CURSOR);

    // Move cursor to bottom
    buffer << capability(CURSOR_POSITION, terminal.m_progress_row, 0);

    // Now, print the actual progress
    for (auto entry : self->m_progress_info) {
      // When there is no total, show an absolute count
      if (entry.second.second < 0) {
        buffer << capability(CLEAR_LINE) << capability(BOLD) << entry.first << capability(RESTORE_STYLE)
               << capability(COLUMN, terminal.m_value_position) << entry.second.first << std::endl;
      }
        // Otherwise, report progress as a bar
      else {
        float ratio = 0;
        if (entry.second.second > 0) {
          ratio = entry.second.first / float(entry.second.second);
          if (ratio > 1)
            ratio = 1.;
        }

        // Build the report string
        std::ostringstream bar;
        bar << entry.second.first << " / " << entry.second.second
            << " (" << std::fixed << std::setprecision(2) << ratio * 100. << "%)";


        // Attach as many spaces as needed to fill the screen width, minus brackets
        bar << std::string(terminal.m_bar_width - bar.str().size(), ' ');

        buffer << capability(BOLD) << entry.first << capability(RESTORE_STYLE) << capability(RESTORE_COLORS)
                  << capability(COLUMN, terminal.m_value_position) << '[';

        // Completed
        auto bar_content = bar.str();
        int completed = bar_content.size() * ratio;

        buffer << capability(SET_FOREGROUND, COLOR_WHITE | COLOR_BRIGHT) << capability(SET_BACKGROUND, COLOR_GREEN)
                  << bar_content.substr(0, completed) << capability(RESTORE_COLORS);

        // Rest
        buffer << capability(SET_FOREGROUND, COLOR_WHITE | COLOR_BRIGHT)
                  << capability(SET_BACKGROUND, COLOR_BLACK | COLOR_BRIGHT)
                  << bar_content.substr(completed) << capability(RESTORE_COLORS);

        // Closing bracket
        buffer << ']' << std::endl;
      }
    }

    // Elapsed time
    buffer << capability(CLEAR_LINE) << capability(BOLD) << "Elapsed" << capability(RESTORE_STYLE)
              << capability(COLUMN, terminal.m_value_position) << elapsed;

    // Restore
    buffer << capability(RESTORE_CURSOR);

    // Flush to cerr
    std::cerr << buffer.str();

    boost::this_thread::sleep(boost::posix_time::seconds(1));
  }
}

} // end SExtractor
