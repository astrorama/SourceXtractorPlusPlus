#include "SEMain/ProgressBar.h"
#include "ModelFitting/ModelFitting/utils.h"

#include <sys/ioctl.h>
#include <iostream>
#include <boost/date_time.hpp>
#include <signal.h>
#include <boost/thread.hpp>

namespace SExtractor {

static struct sigaction sigterm;
static std::map<int, struct sigaction> prev_signal;

/**
 * Intercept several terminating signals so the terminal style can be restored
 */
static void handleTerminatingSignal(int s) {
  ProgressBar::getInstance()->restoreTerminal();

  // Call the previous handler
  ::sigaction(s, &prev_signal[s], nullptr);
  ::raise(s);
}


static const std::string CSI = {"\033["};
static const std::string SGR_RESET = {"\033[0m"};
static const std::string SGR_BOLD = {"\033[1m"};
static const std::string SGR_BG_GREEN = {"\033[42m"};
static const std::string SGR_BG_GRAY = {"\033[100m"};
static const std::string SGR_FG_WHITE = {"\033[37m"};
static const std::string SGR_BG_RESET = {"\033[49m"};
static const std::string SGR_FG_RESET = {"\033[39m"};
static const std::string HIDE_CURSOR{"\033[?25l"};
static const std::string SHOW_CURSOR{"\033[?25h"};
static const std::string CLEAR_LINE{"\033[2K"};
static const std::string SAVE_CURSOR{"\0337"};
static const std::string RESTORE_CURSOR{"\0338"};
static const std::string SWAP_SCREEN_BUFFER{"\033[?1049h"};
static const std::string RESTORE_SCREEN_BUFFER{"\033[?1049l"};


ProgressBar::ProgressBar()
  : m_started{boost::posix_time::second_clock::local_time()} {

  // C++ guarantees that a static object is initialized once, and in a thread-safe manner
  // As this class is a singleton, we can do this here
  ::memset(&sigterm, 0, sizeof(sigterm));

  sigterm.sa_handler = handleTerminatingSignal;
  ::sigaction(SIGINT, &sigterm, &prev_signal[SIGINT]);
  ::sigaction(SIGTERM, &sigterm, &prev_signal[SIGTERM]);
  ::sigaction(SIGABRT, &sigterm, &prev_signal[SIGABRT]);
  ::sigaction(SIGSEGV, &sigterm, &prev_signal[SIGSEGV]);
  ::sigaction(SIGHUP, &sigterm, &prev_signal[SIGHUP]);

  // Swap screen buffer
  std::cerr << SWAP_SCREEN_BUFFER;
}

ProgressBar::~ProgressBar() {
}

std::shared_ptr<ProgressBar> ProgressBar::getInstance() {
  static std::shared_ptr<ProgressBar> progress_bar_singleton{new ProgressBar};
  return progress_bar_singleton;
}

void ProgressBar::restoreTerminal() {
  // Restore full scroll and  attributes
  std::cerr << "\033[r\033[0" << SHOW_CURSOR << std::endl;
  // Restore screen buffer
  std::cerr << RESTORE_SCREEN_BUFFER;
  std::cerr.flush();
}

void ProgressBar::prepareTerminal() {
  // Reserve the bottom side for the progress report
  struct winsize w;
  ioctl(STDERR_FILENO, TIOCGWINSZ, &w);

  m_progress_row = w.ws_row - m_progress_info.size();
  // Scroll up so lines are preserved
  std::cerr << "\033[" << m_progress_info.size() << 'S';
  // Block scrolling
  std::cerr << "\033[0;" << m_progress_row - 1 << 'r';

  // Precompute the width of the bars (remember brackets!)
  m_bar_width = w.ws_col - m_value_position - 2;
}

void ProgressBar::update(const std::map<std::string, std::pair<int, int>>& info) {
  this->ProgressReporter::update(info);

  // On first call, prepare and spawn the progress report block
  if (!m_progress_thread) {
    // Put always the value at the same distance: length of the longest attribute + space + (space|[) (starts at 1!)
    size_t max_attr_len = 0;

    for (auto& e : info) {
      max_attr_len = std::max(max_attr_len, e.first.size());
    }

    m_value_position = max_attr_len + 3;

    // Prepare the scrolling area
    prepareTerminal();

    // Start printer
    m_progress_thread = make_unique<boost::thread>(ProgressBar::printThread);
  }
}

void ProgressBar::done() {
  this->ProgressReporter::done();
  if (m_progress_thread)
    m_progress_thread->join();
  restoreTerminal();
}

void ProgressBar::printThread() {
  auto self = ProgressBar::getInstance();

  while (!self->m_done) {
    auto now = boost::posix_time::second_clock::local_time();
    auto elapsed = now - self->m_started;

    // Store position and attributes, hide cursor
    std::cerr << SAVE_CURSOR << HIDE_CURSOR;

    // Move cursor to bottom
    std::cerr << CSI << self->m_progress_row << ";0H";

    // Now, print the actual progress
    for (auto entry : self->m_progress_info) {
      // When there is no total, show an absolute count
      if (entry.second.second < 0) {
        std::cerr << CLEAR_LINE << SGR_BOLD << entry.first << SGR_RESET << CSI << self->m_value_position << 'G' << ' '
                  << entry.second.first << std::endl;
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
        bar << std::string(self->m_bar_width - bar.str().size(), ' ');

        std::cerr << SGR_BOLD << entry.first << SGR_RESET << CSI << self->m_value_position << 'G' << '[';

        // Completed
        auto bar_content = bar.str();
        int completed = bar_content.size() * ratio;

        std::cerr << SGR_FG_WHITE << SGR_BG_GREEN << bar_content.substr(0, completed) << SGR_BG_RESET << SGR_FG_RESET;

        // Rest
        std::cerr << SGR_FG_WHITE << SGR_BG_GRAY << bar_content.substr(completed) << SGR_BG_RESET << SGR_FG_RESET;

        // Closing bracket
        std::cerr << ']' << std::endl;
      }
    }

    // Elapsed time
    std::cerr << CLEAR_LINE << SGR_BOLD << "Elapsed" << CSI << self->m_value_position << "G " << SGR_RESET << elapsed;

    // Restore
    std::cerr << RESTORE_CURSOR;

    boost::this_thread::sleep(boost::posix_time::seconds(1));
  }
}

} // end SExtractor
