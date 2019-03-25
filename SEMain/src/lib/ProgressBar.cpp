#include <sys/ioctl.h>
#include <iostream>
#include <boost/date_time.hpp>
#include <signal.h>
#include "SEMain/ProgressBar.h"

namespace SExtractor {

static struct sigaction sigint_new, sigint_prev, sigwinch;

static void handleSigint(int s, siginfo_t *, void *) {
  ProgressBar::getInstance()->restoreTerminal();

  // Call the previous handler
  ::sigaction(s, &sigint_prev, nullptr);
  ::raise(s);
}

static void handleSigwinch(int) {
  ProgressBar::getInstance()->updateTerminal();
  ProgressBar::getInstance()->print();
}

static const std::string CSI = {"\033["};
static const std::string SGR_RESET = {"\033[0m"};
static const std::string SGR_BOLD = {"\033[1m"};
static const std::string SGR_BG_GREEN = {"\033[42m"};
static const std::string SGR_BG_RESET = {"\033[49m"};
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
  ::memset(&sigint_new, 0, sizeof(sigint_new));
  sigint_new.sa_flags = SA_SIGINFO;
  sigint_new.sa_sigaction = handleSigint;
  ::sigaction(SIGINT, &sigint_new, &sigint_prev);

  ::memset(&sigwinch, 0, sizeof(sigwinch));
  sigwinch.sa_handler = handleSigwinch;
  ::sigaction(SIGWINCH, &sigwinch, nullptr);

  // Swap screen buffer
  std::cerr << SWAP_SCREEN_BUFFER;
}

ProgressBar::~ProgressBar() {
  restoreTerminal();
}

std::shared_ptr<ProgressBar> ProgressBar::getInstance() {
  static std::shared_ptr<ProgressBar> progress_bar_singleton{new ProgressBar};
  return progress_bar_singleton;
}

void ProgressBar::restoreTerminal() {
  // Restore full scroll, attributes, and jump to the end
  std::cerr << "\033[r\033[0\033[" << m_progress_row + m_progress_info.size() << ";0H" << SHOW_CURSOR << std::endl;
  // Restore screen buffer
  std::cerr << RESTORE_SCREEN_BUFFER;
}

void ProgressBar::updateTerminal() {
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

void ProgressBar::setElements(const std::vector<std::string>& entries) {
  // Put always the value at the same distance: length of the longest attribute + space + (space|[) (starts at 1!)
  size_t max_attr_len = 0;
  m_progress_info.clear();

  for (auto& e : entries) {
    max_attr_len = std::max(max_attr_len, e.size());
    m_progress_info[e] = std::make_pair(0, -1);
  }

  m_value_position = max_attr_len + 3;

  updateTerminal();
  print();
}

void ProgressBar::print(bool /*done*/) {
  auto now = boost::posix_time::second_clock::local_time();
  auto elapsed = now - m_started;

  // Store position and attributes, hide cursor
  std::cerr << SAVE_CURSOR << HIDE_CURSOR;

  // Move cursor to bottom
  std::cerr << CSI << m_progress_row << ";0H";

  // Now, print the actual progress
  for (auto entry : m_progress_info) {
    // Entry label
    std::cerr << CLEAR_LINE << SGR_BOLD << entry.first << SGR_RESET;
    std::cerr << CSI << m_value_position << 'G';

    // When there is no total, show an absolute count
    if (entry.second.second < 0) {
      std::cerr << ' ' << entry.second.first << std::endl;
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
      bar << std::string(m_bar_width - bar.str().size(), ' ');

      std::cerr << '[';

      // Completed
      auto bar_content = bar.str();
      int completed = bar_content.size() * ratio;

      std::cerr << SGR_BG_GREEN << bar_content.substr(0, completed) << SGR_BG_RESET;

      // Rest
      std::cerr << bar_content.substr(completed);

      // Closing bracket
      std::cerr << ']' << std::endl;
    }
  }

  // Elapsed time
  std::cerr << CLEAR_LINE << SGR_BOLD << "Elapsed" << CSI << m_value_position << "G " << SGR_RESET << elapsed;

  // Restore
  std::cerr << RESTORE_CURSOR;
}

} // end SExtractor
