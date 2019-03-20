#include <sys/ioctl.h>
#include <iostream>
#include <boost/date_time.hpp>
#include <signal.h>
#include "SEMain/ProgressBar.h"

namespace SExtractor {

static struct sigaction sigint_prev = {0}, sigint_new = {0};

static const char CURSOR_NEXT_LINE = 'E';
static const char CURSOR_PREV_LINE = 'F';
static const std::string CSI = {"\033["};
static const std::string SGR_RESET = {"\033[0m"};
static const std::string SGR_BOLD = {"\033[1m"};
static const std::string SGR_BG_GREEN = {"\033[42m"};
static const std::string SGR_BG_RESET = {"\033[49m"};
static const std::string INDENT{' ', 4};
static const std::string HIDE_CURSOR{"\033[?25l"};
static const std::string SHOW_CURSOR{"\033[?25h"};
static const std::string CLEAR_LINE{"\033[2K"};
static const std::string SAVE_CURSOR{"\0337"};
static const std::string RESTORE_CURSOR{"\0338"};


ProgressBar::ProgressBar(std::ostream& out, const std::initializer_list<std::string>& entries)
  : ProgressPrinter{entries}, m_out{out}, m_started{boost::posix_time::second_clock::local_time()} {

  // Reserve the bottom side for the progress report
  struct winsize w;
  ioctl(STDERR_FILENO, TIOCGWINSZ, &w);

  m_progress_row = w.ws_row - m_progress_info.size();
  // Scroll up so lines are preserved
  m_out << "\033[" << m_progress_info.size() << 'S';
  // Block scrolling
  m_out << "\033[0;" << m_progress_row - 1 << 'r';

  // Put always the value at the same distance: length of the longest attribute + space + (space|[) (starts at 1!)
  size_t max_attr_len = 0;
  for (auto& e : entries) {
    max_attr_len = std::max(max_attr_len, e.size());
  }

  m_value_position = max_attr_len + 3;

  // Precompute the width of the bars (remember brackets!)
  m_bar_width = w.ws_col - m_value_position - 2;

  // First display!
  print();
}

ProgressBar::~ProgressBar() {
  // Restore full scroll, attributes, and jump to the end
  m_out << "\033[r\033[0\033[" << m_progress_row + m_progress_info.size() << ";0H" << SHOW_CURSOR << std::endl;
}

void ProgressBar::print(bool /*done*/) {
  auto now = boost::posix_time::second_clock::local_time();
  auto elapsed = now - m_started;

  // Store position and attributes, hide cursor
  m_out << SAVE_CURSOR << HIDE_CURSOR;

  // Move cursor to bottom
  m_out << CSI << m_progress_row << ";0H";

  // Now, print the actual progress
  for (auto entry : m_progress_info) {
    // Entry label
    m_out << CLEAR_LINE << SGR_BOLD << entry.first << SGR_RESET;
    m_out << CSI << m_value_position << 'G';

    // When there is no total, show an absolute count
    if (entry.second.second < 0) {
      m_out << ' ' << entry.second.first << std::endl;
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

      m_out << '[';

      // Completed
      auto bar_content = bar.str();
      int completed = bar_content.size() * ratio;

      m_out << SGR_BG_GREEN << bar_content.substr(0, completed) << SGR_BG_RESET;

      // Rest
      m_out << bar_content.substr(completed);

      // Closing bracket
      m_out << ']' << std::endl;
    }
  }

  // Elapsed time
  m_out << CLEAR_LINE << SGR_BOLD << "Elapsed" << CSI << m_value_position << "G " << SGR_RESET << elapsed;

  // Restore
  m_out << RESTORE_CURSOR;
}

} // end SExtractor
