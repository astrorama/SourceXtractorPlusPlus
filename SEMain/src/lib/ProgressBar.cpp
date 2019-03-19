#include <sys/ioctl.h>
#include <iostream>
#include <boost/date_time.hpp>
#include "SEMain/ProgressBar.h"

namespace SExtractor {

static const char ESC = 0x1B;
static const char CSI[] = {ESC, '[', '\0'};
static const char CURSOR_PREV_LINE = 'F';
static const char SGR_RESET[] = {'0', 'm', '\0'};
static const char SGR_BOLD[] = {'1', 'm', '\0'};
static const char SGR_BG_GREEN[] = {'4', '2', 'm', '\0'};
static const char SGR_BG_RESET[] = {'4', '9', 'm', '\0'};
static const std::string INDENT{' ', 4};
static const std::string HIDE_CURSOR{"\033[?25l"};
static const std::string SHOW_CURSOR{"\033[?25h"};


ProgressBar::ProgressBar(std::ostream& out) : m_out{out}, m_started{boost::posix_time::second_clock::local_time()} {
}

void ProgressBar::print(bool done) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  auto now = boost::posix_time::second_clock::local_time();
  auto elapsed = now - m_started;

  int nlines = 0;

  m_out << HIDE_CURSOR;

  for (auto entry : m_progress_info) {
    // When there is no total, log an absolute count
    if (entry.second.second < 0) {
      m_out << CSI << SGR_BOLD << entry.first << CSI << SGR_RESET << std::endl
            << INDENT << entry.second.first
            << std::endl;
    }
      // Otherwise, report progress
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

      // Attach as many spaces as needed to fill the screen width, minus indent, minus brackets
      bar << std::string(w.ws_col - bar.str().size() - INDENT.size() - 2, ' ');

      // Label
      m_out << CSI << SGR_BOLD << entry.first << CSI << SGR_RESET << std::endl;

      // Indent + opening bracket
      m_out << INDENT << '[';

      // Completed
      auto bar_content = bar.str();
      int completed = bar_content.size() * ratio;

      m_out << CSI << SGR_BG_GREEN << bar_content.substr(0, completed) << CSI << SGR_BG_RESET;

      // Rest
      m_out << bar_content.substr(completed);

      // Closing bracket
      m_out << ']' << std::endl;
    }
    nlines += 2;
  }

  m_out << elapsed << SHOW_CURSOR;

  // Rewind
  if (!done) {
    m_out << CSI << nlines << CURSOR_PREV_LINE;
  } else {
    m_out << std::endl;
  }
}

} // end SExtractor
