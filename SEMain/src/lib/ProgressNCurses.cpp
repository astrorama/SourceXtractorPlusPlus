#include "SEMain/ProgressNCurses.h"
#include "ModelFitting/ModelFitting/utils.h"

#include <sys/ioctl.h>
#include <signal.h>
#include <ncurses.h>
#include <term.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <condition_variable>

namespace SExtractor {

// Signal handlers
static struct sigaction sigterm;
static std::map<int, struct sigaction> prev_signal;

// Forward declaration of signal handlers
static void handleTerminatingSignal(int s);


/**
 * @brief Wrap the terminal into a singleton
 */
class Screen : public boost::noncopyable {
public:
  // Any function that access the screen should hold this mutex
  std::recursive_mutex m_mutex;

  virtual ~Screen() {
    terminate();
    if (m_signal_monitor && m_signal_monitor->joinable())
      m_signal_monitor->join();
  }

  /**
   * Initialize the screen
   * @param outfd
   *    File descriptor to use for output
   * @param infd
   *    File descriptor to use for input
   */
  void initialize(FILE *outfd, FILE *infd) {
    // Start monitor
    m_signal_monitor = make_unique<boost::thread>(std::bind(&Screen::signalMonitor, this));
    // Setup signal handlers
    ::memset(&sigterm, 0, sizeof(sigterm));

    sigterm.sa_handler = &handleTerminatingSignal;
    ::sigaction(SIGINT, &sigterm, &prev_signal[SIGINT]);
    ::sigaction(SIGTERM, &sigterm, &prev_signal[SIGTERM]);
    ::sigaction(SIGABRT, &sigterm, &prev_signal[SIGABRT]);
    ::sigaction(SIGSEGV, &sigterm, &prev_signal[SIGSEGV]);
    ::sigaction(SIGHUP, &sigterm, &prev_signal[SIGHUP]);

    // Enter ncurses
    SCREEN *term = newterm(nullptr, outfd, infd);
    set_term(term);

    // Hide cursor
    curs_set(0);

    // Get input without echo, but leave Ctrl+<Key> to the terminal
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    // Setup colors
    use_default_colors();
    start_color();

    // Refresh screen
    wrefresh(stdscr);
  }

  /**
   * Exit ncurses
   */
  void terminate() {
    // Exit ncurses
    endwin();
    // Kill monitor
    if (m_signal_monitor)
      m_signal_monitor->interrupt();
    m_signal_cv.notify_all();
    // Restore signal handlers
    ::sigaction(SIGINT, &prev_signal[SIGINT], nullptr);
    ::sigaction(SIGTERM, &prev_signal[SIGTERM], nullptr);
    ::sigaction(SIGABRT, &prev_signal[SIGABRT], nullptr);
    ::sigaction(SIGSEGV, &prev_signal[SIGSEGV], nullptr);
    ::sigaction(SIGHUP, &prev_signal[SIGHUP], nullptr);
    // Clean up callbacks
    std::lock_guard<std::recursive_mutex> s_lock(m_mutex);
    m_signal_callbacks.clear();
  }

  /**
   * Return a new color pair with the given foreground and background
   */
  short initColor(short fg, short bg) {
    init_pair(m_color_idx, fg, bg);
    return m_color_idx++;
  }

  typedef std::function<void(int)> SignalCallback;

  /**
   * Add a termination callback
   */
  void addSignalCallback(SignalCallback f) {
    std::lock_guard<std::recursive_mutex> s_lock(m_mutex);
    m_signal_callbacks.emplace_back(f);
  }

  /**
   * Call the registered callbacks
   * @param s
   *    The signal that triggered the termination
   * @note
   *    A signal handler can not do much, as only reentrant functions may be called safely.
   *    So we use a monitor thread that will call the callbacks from outside the handler.
   */
  void callbacks(int s) {
    m_signal = s;
    m_signal_cv.notify_all();
    std::unique_lock<std::mutex> resent_lock(m_resent_mutex);
    m_resent_cv.wait(resent_lock);
  }

private:
  short m_color_idx = 1;
  std::vector<SignalCallback> m_signal_callbacks;
  std::unique_ptr<boost::thread> m_signal_monitor;
  std::mutex m_signal_mutex, m_resent_mutex;
  std::condition_variable m_signal_cv, m_resent_cv;
  std::atomic_int m_signal;

  void signalMonitor(void) {
    std::unique_lock<std::mutex> signal_lock(m_signal_mutex);
    m_signal_cv.wait(signal_lock, [this](){return m_signal || boost::this_thread::interruption_requested();});
    if (m_signal) {
      for (auto &c : m_signal_callbacks) {
        c(m_signal);
      }
      m_resent_cv.notify_all();
    }
  }
};

/** Screen singleton */
static Screen s_screen;

/**
 * Intercept several terminating signals so the terminal style can be restored
 */
static void handleTerminatingSignal(int s) {
  // Restore handler (so if we get stuck somewhere, and second
  // signal occurs, like a SIGTERM, the process is killed for good)
  ::sigaction(s, &prev_signal[s], nullptr);
  // Callbacks
  s_screen.callbacks(s);
  // Call the previous handler
  ::raise(s);
}

/**
 * Custom implementation of a stream buffer, so std::cerr can be transparently redirected into
 * the ncurses pad
 */
class LogWidget : public std::streambuf {
private:
  WINDOW *m_pad, *m_scroll;
  // Screen coordinates!
  int m_display_height, m_display_width;
  int m_display_y, m_display_x;
  // Number of total lines being written so far
  int m_written_lines;
  // Last line being *displayed*
  int m_active_line;
  // Colors
  int m_scroll_bar_color, m_scroll_ind_color;

  static const int BUFFER_INCREASE_STEP_SIZE = 10;

public:

  /**
   * Constructor
   * @param display_height
   *    Displayed height on the screen. The actual pad height is handled internally.
   * @param display_width
   *    Displayed width on the screen.
   * @param display_y
   *    Y position on the screen.
   * @param display_x
   *    X position on the screen.
   */
  LogWidget(int display_height, int display_width, int display_y, int display_x)
    : m_pad(newpad(BUFFER_INCREASE_STEP_SIZE, display_width - 1)),
      m_scroll(newpad(display_height, 1)),
      m_display_height(display_height), m_display_width(display_width), m_display_y(display_y), m_display_x(display_x),
      m_written_lines(0), m_active_line(0) {
    scrollok(m_pad, TRUE);
    m_scroll_bar_color = s_screen.initColor(COLOR_BLACK, COLOR_WHITE);
    m_scroll_ind_color = s_screen.initColor(COLOR_WHITE, COLOR_WHITE);
  }

  /**
   * Destructor
   */
  virtual ~LogWidget() {
    delwin(m_pad);
    delwin(m_scroll);
  }

  /**
   * Called when the buffer is overflowed, so the character c is written to the destination
   * @note This is called by the stream, so we need to lock the screen
   */
  std::streambuf::int_type overflow(std::streambuf::int_type c) override {
    std::lock_guard<std::recursive_mutex> lock(s_screen.m_mutex);

    if (c != traits_type::eof()) {
      if (c == '\n') {
        // If the current line is the last one, follow the log
        if (m_active_line == m_written_lines) {
          ++m_active_line;
        }
        ++m_written_lines;
        // Increase buffer if we ran out of lines on the pad
        if (getmaxy(m_pad) <= m_written_lines) {
          wresize(m_pad, m_written_lines + BUFFER_INCREASE_STEP_SIZE, m_display_width);
        }
      }
      waddch(m_pad, c);
    }
    return c;
  }

  /**
   * Called so synchronize the buffer. This implementation will refresh the display.
   * @note This is called by the stream, so we need to lock the screen
   */
  int sync() override {
    std::lock_guard<std::recursive_mutex> lock(s_screen.m_mutex);
    drawLog();
    drawScroll();
    doupdate();
    return 0;
  }

  /**
   * Update the screen size
   */
  void resize(int display_height, int display_width) {
    m_display_height = display_height;
    m_display_width = display_width;

    // Resize to make place for the new width only if it is bigger.
    // Note that the pad height depends on the number of written lines, not displayed size!
    if (display_width - 1 > getmaxx(m_pad)) {
      wresize(m_pad, getmaxy(m_pad), display_width - 1);
    }
    sync();
  }

  /**
   * Allow to scroll the pad
   * @note Scroll is a ncurses macro, so we can not use it as a name :(
   */
  void move(int d) {
    m_active_line += d;
    if (m_active_line > getcury(m_pad) + 1) {
      m_active_line = getcury(m_pad) + 1;
    } else if (m_active_line < 0) {
      m_active_line = 0;
    } else if (m_written_lines > m_display_height && m_active_line < m_display_height) {
      m_active_line = m_display_height;
    } else if (m_written_lines < m_display_height && m_active_line < m_written_lines) {
      m_active_line = m_written_lines;
    }
    sync();
  }

  /**
   * Dump into a vector the content of the buffer
   */
  std::vector<std::string> getText() {
    // Scan line by line
    std::vector<std::string> term_lines;
    for (int i = 0; i < m_written_lines; ++i) {
      // Note: We do not want the '\0' to be part of the final string, so we use the string constructor to prune those
      std::vector<char> buffer(m_display_width + 1, '\0');
      mvwinnstr(m_pad, i, 0, buffer.data(), m_display_width - 2);
      term_lines.push_back(buffer.data());
      boost::algorithm::trim(term_lines.back());
    }
    // Prune trailing empty lines
    while (!term_lines.empty() && term_lines.back().empty()) {
      term_lines.pop_back();
    }
    return term_lines;
  }

private:
  /**
   * Draw scroll marker
   */
  void drawScroll() const {
    werase(m_scroll);

    int max_selectable_line = m_written_lines;
    int min_selectable_line = std::min(m_written_lines, m_display_height);
    int displayed_line_offset = m_active_line - min_selectable_line;
    float p = displayed_line_offset / float(max_selectable_line - min_selectable_line);

    int scroll_marker_pos = p * (m_display_height - 1);
    for (int i = 0; i < m_display_height; ++i) {
      if (i == scroll_marker_pos)
        waddch(m_scroll, '|' | COLOR_PAIR(m_scroll_ind_color));
      else
        waddch(m_scroll, ACS_CKBOARD | COLOR_PAIR(m_scroll_bar_color));
    }
    pnoutrefresh(m_scroll,
                 0, 0,
                 m_display_y, m_display_x + m_display_width - 2,
                 m_display_y + m_display_height - 1, m_display_x + m_display_width - 2
    );
  }

  /**
   * Draw log lines
   */
  void drawLog() const {
    int pad_y = std::max(m_active_line - m_display_height, 0);
    pnoutrefresh(m_pad,
                 pad_y, 0, // Pad coordinates
                 m_display_y, m_display_x, // Start screen coordinates
                 m_display_y + m_display_height - 1, m_display_x + m_display_width - 3 // End screen coordinates
    );
  }
};

/**
 * @brief Set of progress bars/information entries
 */
class ProgressWidget : public boost::noncopyable {
public:
  /**
   * Constructor
   * @param height
   *    Height in lines
   * @param width
   *    Width in columns
   * @param y
   *    Start line
   * @param x
   *    Start column
   */
  ProgressWidget(int height, int width, int y, int x)
    : m_window(newwin(height, width, y, x)), m_started(std::chrono::steady_clock::now()) {
    m_color_done = s_screen.initColor(COLOR_WHITE, COLOR_GREEN);
    m_color_progress = s_screen.initColor(COLOR_WHITE, COLOR_BLACK);
  }

  /**
   * Destructor
   */
  ~ProgressWidget() {
    delwin(m_window);
  }

  /**
   * Move the progress widget
   * @param y
   *    Line
   * @param x
   *    Column
   */
  void move(int y, int x) {
    mvwin(m_window, y, x);
  }

  /**
   * Resize the progress widget
   * @param height
   *    The height in lines
   * @param width
   *    The width in columns
   */
  void resize(int height, int width) {
    wresize(m_window, height, width);
  }

  /**
   * @return The height of the progress widget
   */
  int getHeight() const {
    return getmaxy(m_window);
  }

  /**
   * Update and redraw the progress information
   */
  void update(const std::map<std::string, std::pair<int, int>>& info) {
    // Precalculate layout, so labels are aligned
    size_t value_position = sizeof("Elapsed");

    for (auto& entry: info) {
      if (entry.first.size() > value_position) {
        value_position = entry.first.size();
      }
    }
    value_position++; // Plus space

    // Width of the bar is the with of the windows - a space - two brackets []
    size_t bar_width = getmaxx(m_window) - 3 - value_position;

    std::lock_guard<std::recursive_mutex> lock(s_screen.m_mutex);

    // Elapsed
    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - m_started;

    // Restore position to the beginning
    werase(m_window);
    wmove(m_window, 0, 0);

    // Now, print the actual progress
    int line = 0;
    for (auto& entry : info) {
      wmove(m_window, line, 0);
      drawProgressLine(value_position, bar_width, line, entry.first, entry.second.second, entry.second.first);
      ++line;
    }

    // Elapsed time
    drawElapsed(value_position, elapsed, line);

    // Flush
    wrefresh(m_window);
    doupdate();
  }

private:
  /**
   * Draw the elapsed counter
   */
  void drawElapsed(size_t value_position, const std::chrono::steady_clock::duration& elapsed, int line) const {
    auto h = std::chrono::duration_cast<std::chrono::hours>(elapsed);
    auto m = std::chrono::duration_cast<std::chrono::minutes>(elapsed - h);
    auto s = std::chrono::duration_cast<std::chrono::seconds>(elapsed - h - m);
    std::ostringstream elapsed_str;
    elapsed_str.fill('0');
    elapsed_str << std::setw(2) << h.count() << ':' << std::setw(2) << m.count() << ':' << std::setw(2) << s.count();

    wattron(m_window, A_BOLD);
    mvwaddstr(m_window, line, 0, "Elapsed");
    wattroff(m_window, A_BOLD);
    mvwaddstr(
      m_window,
      line, value_position + 1,
      elapsed_str.str().c_str()
    );
  }

  /**
   * Draw the set of progress bar/report on the bottom of the screen
   */
  void drawProgressLine(size_t value_position, size_t bar_width, int line, const std::string& label,
                        int total, int done) const {

    // Total number is unknown
    if (total <= 0) {
      wattron(m_window, A_BOLD);
      waddstr(m_window, label.c_str());
      wattroff(m_window, A_BOLD);
      wmove(m_window, line, value_position + 1);
      wprintw(m_window, "%d", done);
      return;
    }

    // Otherwise, report progress as a bar
    float ratio = done / static_cast<float>(total);
    // This can happens sometimes, as a measurement could be notified before the deblending, for instance
    if (ratio > 1)
      ratio = 1.;

    // Build the report string
    std::ostringstream bar;
    bar << done << " / " << total << " (" << std::fixed << std::setprecision(2) << ratio * 100. << "%)";

    // Attach as many spaces as needed to fill the screen width, minus brackets
    bar << std::string(bar_width - bar.str().size(), ' ');

    // Print label
    wattron(m_window, A_BOLD);
    waddstr(m_window, label.c_str());
    wattroff(m_window, A_BOLD);
    mvwaddch(m_window, line, value_position, '[');

    // Completed
    auto bar_content = bar.str();
    int completed = bar_content.size() * ratio;

    wattron(m_window, COLOR_PAIR(m_color_done));
    waddstr(m_window, bar_content.substr(0, completed).c_str());
    wattroff(m_window, COLOR_PAIR(m_color_done));

    // Rest
    wattron(m_window, COLOR_PAIR(m_color_progress));
    waddstr(m_window, bar_content.substr(completed).c_str());
    wattroff(m_window, COLOR_PAIR(2));

    // Closing bracket
    waddch(m_window, ']');
  }

  WINDOW *m_window;
  std::chrono::steady_clock::time_point m_started;
  short m_color_done, m_color_progress;
};

/**
 * @brief Dashboard for reporting SExtractor progress
 */
class ProgressNCurses::Dashboard : public boost::noncopyable {
public:
  /**
   * Constructor
   */
  Dashboard() : m_done(false) {
    std::lock_guard<std::recursive_mutex> lock(s_screen.m_mutex);

    s_screen.initialize(stdout, stdin);
    m_log_widget = make_unique<LogWidget>(LINES - 1, COLS, 0, 0);
    m_cerr_original_rdbuf = std::cerr.rdbuf();
    std::cerr.rdbuf(m_log_widget.get());

    m_progress_widget = make_unique<ProgressWidget>(1, COLS, LINES - 1, 0);

    s_screen.addSignalCallback(std::bind(&Dashboard::signalCallback, this, std::placeholders::_1));

    m_ui_thread = make_unique<boost::thread>(std::bind(&Dashboard::uiThread, this));
  }

  /**
   * Destructor
   */
  virtual ~Dashboard() {
    terminate();
  }

  /**
   * Shutdown the UI
   */
  void terminate() {
    // Terminate UI thread
    m_done = true;
    m_ui_thread->interrupt();
    m_ui_thread->join();

    std::lock_guard<std::recursive_mutex> s_lock(s_screen.m_mutex);
    auto log_lines = m_log_widget->getText();

    // Terminate ncurses
    s_screen.terminate();

    // Dump log to the restored stderr
    std::cerr.rdbuf(m_cerr_original_rdbuf);
    for (auto& l : log_lines) {
      std::cerr << l << std::endl;
    }
  }

  /**
   * Update the progress information
   */
  void update(const std::map<std::string, std::pair<int, int>>& info) {
    std::lock_guard<std::mutex> p_lock(m_progress_info_mutex);

    // Resize if needed the widgets
    if (info.size() != m_progress_info.size()) {
      std::lock_guard<std::recursive_mutex> s_lock(s_screen.m_mutex);
      m_progress_widget->move(LINES - info.size() - 1, 0);
      m_progress_widget->resize(info.size() + 1, COLS);
      m_log_widget->resize(LINES - m_progress_widget->getHeight(), COLS);
    }

    m_progress_info = info;
  }

private:
  std::streambuf *m_cerr_original_rdbuf;
  std::unique_ptr<LogWidget> m_log_widget;
  std::unique_ptr<ProgressWidget> m_progress_widget;
  std::unique_ptr<boost::thread> m_ui_thread;
  std::map<std::string, std::pair<int, int>> m_progress_info;
  std::atomic_bool m_done;
  std::mutex m_progress_info_mutex;

  /**
   * Called when the screen has been resized
   */
  void notifyResize() {
    werase(stdscr);
    wrefresh(stdscr);
    m_progress_widget->move(LINES - m_progress_info.size() - 1, 0);
    m_progress_widget->update(m_progress_info);
    m_log_widget->resize(LINES - m_progress_widget->getHeight(), COLS);
    m_log_widget->sync();
  }

  /**
   * UI loop
   */
  void uiThread() {
    while (!m_done && !boost::this_thread::interruption_requested()) {
      timeout(1000);
      int key = wgetch(stdscr);
      {
        std::lock_guard<std::recursive_mutex> s_lock(s_screen.m_mutex);
        switch (key) {
          case KEY_DOWN:
            m_log_widget->move(1);
            break;
          case KEY_UP:
            m_log_widget->move(-1);
            break;
          case KEY_NPAGE:
            m_log_widget->move(LINES);
            break;
          case KEY_PPAGE:
            m_log_widget->move(-LINES);
            break;
          case KEY_RESIZE:
            notifyResize();
            break;
        }
      }
      std::lock_guard<std::mutex> p_lock(m_progress_info_mutex);
      m_progress_widget->update(m_progress_info);
    }
  }

  /**
   * Callback for termination signals. It is basically like terminate, but
   * without waiting for the uiThread, as the signal may have been triggered there
   */
  void signalCallback(int) {
    std::lock_guard<std::recursive_mutex> s_lock(s_screen.m_mutex);
    auto log_lines = m_log_widget->getText();

    // Terminate ncurses
    s_screen.terminate();

    // Dump log to the restored stderr
    std::cerr.rdbuf(m_cerr_original_rdbuf);
    for (auto& l : log_lines) {
      std::cerr << l << std::endl;
    }
  }
};


ProgressNCurses::ProgressNCurses() {
  m_dashboard = make_unique<Dashboard>();
}

ProgressNCurses::~ProgressNCurses() {
}

bool ProgressNCurses::isTerminalCapable() {
  return isatty(fileno(stdout));
}

void ProgressNCurses::handleMessage(const std::map<std::string, std::pair<int, int>>& info) {
  if (m_dashboard)
    m_dashboard->update(info);
}

void ProgressNCurses::handleMessage(const bool& done) {
  if (done && m_dashboard)
    m_dashboard.reset(nullptr);
}


} // end SExtractor
