/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "AlexandriaKernel/memory_tools.h"
#include "SEMain/ProgressNCurses.h"

#include <poll.h>
#include <semaphore.h>
#include <ncurses.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <csignal>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <boost/algorithm/string/trim.hpp>
#include <boost/thread.hpp>


using Euclid::make_unique;


namespace SourceXtractor {

// Signal handlers
static struct sigaction                sigterm_action;
static struct sigaction                sigstop_action;
static struct sigaction                sigcont_action;
static struct sigaction                sigwich_action;
static std::map<int, struct sigaction> prev_signal;

// Used for sending signals to the UI thread
static int signal_fds[2];

// Used by the UI thread to notify that is is done
struct ncurses_done_t {
  sem_t m_semaphore;
  ncurses_done_t() {
    sem_init(&m_semaphore, 0, 1);
  }
};
static ncurses_done_t ncurses_done;

// Forward declaration of signal handlers
static void handleTerminatingSignal(int s);
static void handleStopSignal(int s);
static void handleContinuationSignal(int s);
static void handleResizeSignal(int);


/**
 * Intercepts writes to a file descriptor old_fd into a pipe, and returns the reading end
 * @param old_fd
 *    File descriptor to intercept
 * @param backup_fd
 *    Duplicate the file descriptor and put it here, so we can restore later
 * @return
 *    A new file descriptor, corresponding now to the read end of the intercepting pipe
 */
static int interceptFileDescriptor(int old_fd, int *backup_fd) {
  int pipe_fds[2];

  *backup_fd = dup(old_fd);
  if (*backup_fd < 0) {
    throw std::system_error(errno, std::generic_category());
  }

  if (pipe(pipe_fds) < 0) {
    throw std::system_error(errno, std::generic_category());
  }

  int flags = fcntl(pipe_fds[0], F_GETFL, 0);
  if (fcntl(pipe_fds[0], F_SETFL, flags | O_NONBLOCK) < 0) {
    throw std::system_error(errno, std::generic_category());
  }

  if (dup2(pipe_fds[1], old_fd) < 0) {
    throw std::system_error(errno, std::generic_category());
  }
  close(pipe_fds[1]);

  return pipe_fds[0];
}

/**
 * But why? You may ask. Because it looks like before Python 3.7, when loading the
 * SourceXtractor++ Python configuration, something would trigger the loading of readline, which in turns
 * tries to get the terminal size, overwriting LINES and COLS and leaving them with the default 80x24
 * (Maybe because we intercept stderr/stdout?)
 * This leaves our ncurses UI in a bad shape, not being able to properly go back to the former state at exiting.
 * Looking at the readline code, it appears like we can avoid a call into tgetent if we override rl_redisplay_function
 */
static void override_rl_display(void) {
}

/**
 * @brief Wrap the terminal into a singleton
 */
class Screen : public boost::noncopyable {
public:

  /**
   * Initialize the ncurses screen
   * @param outfd
   *    File descriptor to use for output
   * @param infd
   *    File descriptor to use for input
   */
  Screen(FILE *outfd, FILE *infd) {
    if (pipe(signal_fds) < 0) {
      throw std::system_error(errno, std::generic_category());
    }

    m_old_redisplay = rl_redisplay_function;
    rl_redisplay_function = override_rl_display;

    // Tell readline to leave the terminal be
    rl_catch_signals = 0;
    rl_deprep_term_function = nullptr;
    rl_prep_term_function = nullptr;

    // It seems like the readline in MacOSX is not the "real" readline, but a compatibility
    // layer which misses some things, like the following:
#ifndef __APPLE__
    rl_catch_sigwinch = 0;
#endif

    // Setup signal handlers
    ::memset(&sigterm_action, 0, sizeof(sigterm_action));
    ::memset(&sigstop_action, 0, sizeof(sigstop_action));
    ::memset(&sigcont_action, 0, sizeof(sigcont_action));
    ::memset(&sigwich_action, 0, sizeof(sigwich_action));

    // Termination
    sigterm_action.sa_handler = &handleTerminatingSignal;
    ::sigaction(SIGINT, &sigterm_action, &prev_signal[SIGINT]);
    ::sigaction(SIGTERM, &sigterm_action, &prev_signal[SIGTERM]);
    ::sigaction(SIGABRT, &sigterm_action, &prev_signal[SIGABRT]);
    ::sigaction(SIGSEGV, &sigterm_action, &prev_signal[SIGSEGV]);
    ::sigaction(SIGHUP, &sigterm_action, &prev_signal[SIGHUP]);

    // bg
    sigstop_action.sa_handler = &handleStopSignal;
    ::sigaction(SIGTSTP, &sigstop_action, &prev_signal[SIGTSTP]);

    // fg
    sigcont_action.sa_handler = &handleContinuationSignal;
    ::sigaction(SIGCONT, &sigcont_action, &prev_signal[SIGCONT]);

    // Resizing
    // Some versions of ncurses handle this by themselves, some other do not, so
    // we do it ourselves in anycase
    sigwich_action.sa_handler = &handleResizeSignal;
    ::sigaction(SIGWINCH, &sigwich_action, &prev_signal[SIGWINCH]);

    // Enter ncurses
    initscr();
    m_screen = newterm(nullptr, outfd, infd);
    set_term(m_screen);

    // Hide cursor
    curs_set(0);

    // Get input without echo, but leave Ctrl+<Key> to the terminal
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    // Setup colors
    use_default_colors();
    start_color();
  }

  /**
   * Exit the ncurses mode
   */
  virtual ~Screen() {
    // Exit ncurses
    endwin();
    delscreen(m_screen);
    rl_redisplay_function = m_old_redisplay;
    // Restore signal handlers
    ::sigaction(SIGINT, &prev_signal[SIGINT], nullptr);
    ::sigaction(SIGTERM, &prev_signal[SIGTERM], nullptr);
    ::sigaction(SIGABRT, &prev_signal[SIGABRT], nullptr);
    ::sigaction(SIGSEGV, &prev_signal[SIGSEGV], nullptr);
    ::sigaction(SIGHUP, &prev_signal[SIGHUP], nullptr);
    ::sigaction(SIGCONT, &prev_signal[SIGCONT], nullptr);
    ::sigaction(SIGWINCH, &prev_signal[SIGWINCH], nullptr);
    close(signal_fds[0]);
    close(signal_fds[1]);
  }

  /**
   * Return a new color pair with the given foreground and background
   */
  short initColor(short fg, short bg) {
    init_pair(m_color_idx, fg, bg);
    return m_color_idx++;
  }

private:
  short m_color_idx = 1;
  SCREEN *m_screen;
  rl_voidfunc_t* m_old_redisplay;
};

/**
 * Intercept several terminating signals so the terminal style can be restored
 *
 * We can only call signal safe functions here: see `man 7 signal-safety`.
 * We use a pipe to communicate the signals, as write is safe.
 * It would be nice to use C++ types for the thread synchronization, but they may not be signal
 * safe. From C++14:
 *      The common subset of the C and C++ languages consists of all declarations, definitions, and expressions
 *      that may appear in a well formed C++ program and also in a conforming C program.
 *      A POF (“plain old function”) is a function that uses only features from this common subset, and that
 *      does not directly or indirectly use any function that is not a POF.
 *      [...]
 *      The behavior of any function other than a POF used as a signal handler in a C++ program is
 *      implementation-defined.
 * Therefore we resort to semaphores (sem_*), which are signal safe
 */
static void handleTerminatingSignal(int s) {
  // Restore handler (so if we get stuck somewhere, and second
  // signal occurs, like a SIGTERM, the process is killed for good)
  sigaction(s, &prev_signal[s], nullptr);

  // Notify
  if (write(signal_fds[1], &s, sizeof(s)) == sizeof(s)) {
    close(signal_fds[1]);
    // Wait for UI thread to be done
#if _POSIX_C_SOURCE >= 200112L
    timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 5;
    sem_timedwait(&ncurses_done.m_semaphore, &timeout);
#else
    // MacOSX does not have timedwait
    int timeout = 5;
    while(timeout > 0 && sem_trywait(&ncurses_done.m_semaphore) < 0) {
        sleep(1);
        --timeout;
    }
#endif
  }

  // Call the previous handler
  raise(s);
}

/**
 * Intercept SIGTSTP (Ctrl+Z)
 */
static void handleStopSignal(int s) {
  // Restore handler
  sigaction(s, &prev_signal[s], nullptr);

  // Exit ncurses
  endwin();

  // Trigger the previous handler
  raise(s);
}

/**
 * Intercept SIGCONT (after fg, for instance)
 */
static void handleContinuationSignal(int) {
  // Restore handlers
  sigaction(SIGCONT, &sigcont_action, nullptr);
  sigaction(SIGTSTP, &sigstop_action, nullptr);
}

/**
 * Intercept SIGWICH (resize)
 */
static void handleResizeSignal(int s) {
  if (write(signal_fds[1], &s, sizeof(s)) < 0) {
    // Just ignore
  }
}

/**
 * Widget into which the logging is redirected
 */
class LogWidget {
private:
  WINDOW* m_pad;
  WINDOW* m_scroll;
  // Screen coordinates!
  int m_display_height;
  int m_display_width;
  int m_display_y;
  int m_display_x;
  // Number of total lines being written so far
  int m_written_lines = 0;
  // Last line being *displayed*
  int m_active_line = 0;
  // Colors
  int m_scroll_bar_color;
  int m_scroll_ind_color;

  static const int BUFFER_INCREASE_STEP_SIZE = 10;
  static const int BUFFER_MAX_SIZE           = 16384;

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
   * @param bar_color
   *    Color for the progress bar
   * @param ind_color
   *    Color for the bar indicator
   */
  LogWidget(int display_height, int display_width, int display_y, int display_x, short bar_color, short ind_color)
    : m_pad(newpad(BUFFER_INCREASE_STEP_SIZE, display_width)),
      m_scroll(newpad(display_height, 1)),
      m_display_height(display_height), m_display_width(display_width), m_display_y(display_y), m_display_x(display_x),
      m_scroll_bar_color(bar_color), m_scroll_ind_color(ind_color) {
    scrollok(m_pad, TRUE);
  }

  /**
   * Destructor
   */
  virtual ~LogWidget() {
    delwin(m_pad);
    delwin(m_scroll);
  }

  LogWidget(const LogWidget&)                  = delete;
  const LogWidget& operator=(const LogWidget&) = delete;

  /**
   * Write into the widget
   */
  void write(const char *data, ssize_t nchars) {
    while (nchars > 0) {
      if (*data == '\n') {
        // If the current line is the last one, follow the log
        if (m_active_line == m_written_lines) {
          ++m_active_line;
        }
        m_written_lines = std::min({m_written_lines + 1, BUFFER_MAX_SIZE});
        // Increase buffer if we ran out of lines on the pad
        if (getmaxy(m_pad) <= m_written_lines) {
          wresize(m_pad, m_written_lines + BUFFER_INCREASE_STEP_SIZE, m_display_width);
        }
      }
      waddch(m_pad, *data);
      ++data;
      --nchars;
    }
    drawLog();
    drawScroll();
  }

  /**
   * Update the screen size
   */
  void resize(int display_height, int display_width) {
    m_display_height = display_height;
    m_display_width = display_width;

    // Resize to make place for the new width only if it is bigger.
    // Note that the pad height depends on the number of written lines, not displayed size!
    if (display_width > getmaxx(m_pad)) {
      wresize(m_pad, getmaxy(m_pad), display_width);
    }
    wresize(m_scroll, display_height, 1);
    drawLog();
    drawScroll();
  }

  /**
   * Allow to scroll the pad
   * @note Scroll is a ncurses macro, so we can not use it as a name :(
   */
  void scrollText(int d) {
    m_active_line += d;
    if (m_active_line > getcury(m_pad) + 1) {
      m_active_line = getcury(m_pad) + 1;
    }
    else if (m_written_lines > m_display_height && m_active_line < m_display_height) {
      m_active_line = m_display_height;
    }
    else if (m_written_lines < m_display_height && m_active_line < m_written_lines) {
      m_active_line = m_written_lines;
    }
    drawLog();
    drawScroll();
  }

  /**
   * Handle key presses (scrolling)
   */
  void handleKeyPress(int key) {
    switch (key) {
      case KEY_DOWN:
        scrollText(1);
        break;
      case KEY_UP:
        scrollText(-1);
        break;
      case KEY_NPAGE:
        scrollText(LINES);
        break;
      case KEY_PPAGE:
        scrollText(-LINES);
        break;
      default:
        break;
    }
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
      term_lines.emplace_back(buffer.data());
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
    float p = std::max(0.f, std::min(1.f, static_cast<float>(displayed_line_offset) /
                                              static_cast<float>(max_selectable_line - min_selectable_line)));

    auto scroll_marker_pos = static_cast<int>(p * static_cast<float>(m_display_height - 1));
    for (int i = 0; i < m_display_height; ++i) {
      if (i == scroll_marker_pos)
        waddch(m_scroll, ACS_CKBOARD | COLOR_PAIR(m_scroll_ind_color));
      else
        waddch(m_scroll, '|' | COLOR_PAIR(m_scroll_bar_color));
    }
    pnoutrefresh(m_scroll,
                 0, 0,
                 m_display_y, m_display_x + m_display_width - 1,
                 m_display_y + m_display_height - 1, m_display_x + m_display_width - 1
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
                 m_display_y + m_display_height - 1, m_display_x + m_display_width - 2 // End screen coordinates
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
   * @param done_color
   *    Color for the done part
   * @param progress_color
   *    Color for the progress bar
   */
  ProgressWidget(int height, int width, int y, int x, short done_color, short progress_color)
      : m_window(newwin(height, width, y, x)), m_done_color(done_color), m_progress_color(progress_color) {}

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
    wnoutrefresh(m_window);
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
    wnoutrefresh(m_window);
  }

  /**
   * @return The height of the progress widget
   */
  unsigned getHeight() const {
    return getmaxy(m_window);
  }

  /**
   * Update and redraw the progress information
   */
  void update(const std::list<ProgressInfo>& info) {
    // Precalculate layout, so labels are aligned
    size_t value_position = sizeof("Elapsed");

    for (auto& entry: info) {
      if (entry.m_label.size() > value_position) {
        value_position = entry.m_label.size();
      }
    }
    value_position++; // Plus space

    // Width of the bar is the with of the windows - a space - two brackets []
    int bar_width = getmaxx(m_window) - 2 - static_cast<int>(value_position);

    // Elapsed
    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - m_started;

    // Restore position to the beginning
    werase(m_window);

    // Now, print the actual progress
    int line = 0;
    for (auto& entry : info) {
      drawProgressLine(static_cast<int>(value_position), bar_width, line, entry.m_label, entry.m_total, entry.m_done);
      ++line;
    }

    // Elapsed time
    drawElapsed(static_cast<int>(value_position), elapsed, line);

    // Flush
    wnoutrefresh(m_window);
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
  void drawProgressLine(int value_position, int bar_width, int line, const std::string& label,
                        int total, int done) const {
    // Label
    wattron(m_window, A_BOLD);
    mvwaddstr(m_window, line, 0, label.c_str());
    wattroff(m_window, A_BOLD);

    // Total number is unknown
    if (total <= 0) {
      mvwprintw(m_window, line, value_position + 1, "%d", done);
      return;
    }

    // Otherwise, report progress as a bar
    float ratio = static_cast<float>(done) / static_cast<float>(total);
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
    mvwaddstr(m_window, line, 0, label.c_str());
    wattroff(m_window, A_BOLD);
    mvwaddch(m_window, line, value_position, '[');

    // Completed
    auto bar_content = bar.str();
    auto completed = static_cast<int>(static_cast<float>(bar_content.size()) * ratio);

    wattron(m_window, COLOR_PAIR(m_done_color));
    waddstr(m_window, bar_content.substr(0, completed).c_str());
    wattroff(m_window, COLOR_PAIR(m_done_color));

    // Rest
    wattron(m_window, COLOR_PAIR(m_progress_color));
    waddstr(m_window, bar_content.substr(completed).c_str());
    wattroff(m_window, COLOR_PAIR(2));

    // Closing bracket
    waddch(m_window, ']');
  }

  WINDOW *m_window;
  std::chrono::steady_clock::time_point m_started = std::chrono::steady_clock::now();
  short m_done_color;
  short m_progress_color;
};

/**
 * The Dashboard wraps the UI screen, which runs on an independent thread to avoid race conditions.
 *
 * ncurses calls are not thread safe, so it is better to contain them within a single thread.
 * Normally, ncurses does a proper setup of signal handling (i.e. SIGINT), so the terminal is returned
 * into its proper state. However, we want to re-print the output after exiting the curses mode, so we need
 * to handle the signals ourselves: catch, exit ncurses, dump log, and delegate to the previously installed handler.
 *
 * @see handleTerminatingSignal
 */
class ProgressNCurses::Dashboard {
private:
  std::unique_ptr<boost::thread> m_ui_thread;
  std::list<ProgressInfo> m_progress_info;
  std::mutex m_progress_info_mutex;

  // stderr intercept
  int   m_stderr_original;
  int   m_stderr_pipe;
  FILE* m_stderr;
  // stdout intercept
  int m_stdout_original;
  int m_stdout_pipe;

  // Used to recover log into the standard output
  std::vector<std::string> m_log_text;

  std::atomic_bool m_trigger_resize{false};
  std::atomic_bool m_exit_loop{false};

  /**
   * Main UI thread. All (almost) ncurses handling should be done here, as it is not thread safe
   */
  void uiThread() {
    sem_wait(&ncurses_done.m_semaphore);
    // SIGTERM, SIGINT and SIGHUP should not be handled by this thread, or we will not be able to properly
    // exit ncurses.
    // Hopefully there should be no SIGABRT or SIGSEGV here. If there were, we will exit but we will not be able
    // to restore the terminal state. Having an abort or a segmentation fault is a bug anyway.
    sigset_t set;
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGHUP);
    pthread_sigmask(SIG_BLOCK, &set, nullptr);
    // Enter ncurses
    ncursesMode();
    // Recover file descriptors
    dup2(m_stderr_original, STDERR_FILENO);
    dup2(m_stdout_original, STDOUT_FILENO);
    // Dump recovered text
    for (const auto& line : m_log_text) {
      std::cerr << line << std::endl;
    }
    sem_post(&ncurses_done.m_semaphore);
  }

  void handleSignal(const struct pollfd& poll_fd, LogWidget& logWidget) {
    if (poll_fd.revents & POLLIN) {
      int signal_no;
      if (read(signal_fds[0], &signal_no, sizeof(signal_no)) == sizeof(signal_no) && signal_no == SIGWINCH) {
        m_trigger_resize = true;
        endwin();
        refresh();
        clear();
      }
      else {
        char buf[64];
        logWidget.write(buf, snprintf(buf, sizeof(buf), "Caught signal %s\n", strsignal(signal_no)));
        m_exit_loop = true;
      }
    }
  }

  void pipeToLog(const struct pollfd& poll_fd, int pipe, LogWidget& out) const {
    if (poll_fd.revents & POLLIN) {
      ssize_t nbytes;
      char buf[64];
      while ((nbytes = read(pipe, &buf, sizeof(buf))) > 0) {
        out.write(buf, nbytes);
      }
    }
  }

  void handleKeyPress(const struct pollfd& poll_fd, LogWidget& logWidget) const {
    if (poll_fd.revents & POLLIN) {
      int key = wgetch(stdscr);
      if (key != KEY_RESIZE) {
        logWidget.handleKeyPress(key);
      }
    }
  }

  /**
   * When we enter we are in ncurses, when we exit we are not (yay RAII)
   */
  void ncursesMode() {
    Screen screen(m_stderr, stdin);

    // Log area
    LogWidget logWidget(
      LINES - 1, COLS, 0, 0,
      screen.initColor(COLOR_WHITE, COLOR_BLACK), screen.initColor(COLOR_WHITE, COLOR_WHITE)
    );

    // Progress widget
    ProgressWidget progressWidget(
      1, COLS, LINES - 1, 0,
      screen.initColor(COLOR_WHITE, COLOR_GREEN), screen.initColor(COLOR_WHITE, COLOR_BLACK)
    );

    // File descriptors to watch for
    struct pollfd poll_fds[] = {
      {m_stderr_pipe, POLLIN, 0},
      {m_stdout_pipe, POLLIN, 0},
      {STDIN_FILENO,  POLLIN, 0},
      {signal_fds[0], POLLIN, 0}
    };

    // Event loop
    m_exit_loop = false;

    do {
      // There has been a signal
      handleSignal(poll_fds[3], logWidget);

      // Resize widgets if needed
      if (m_trigger_resize) {
        std::lock_guard<std::mutex> p_lock(m_progress_info_mutex);
        progressWidget.move(static_cast<int>(LINES - m_progress_info.size() - 1), 0);
        progressWidget.resize(static_cast<int>(m_progress_info.size() + 1), COLS);
        logWidget.resize(LINES - progressWidget.getHeight(), COLS);
        m_trigger_resize = false;
      }

      // There is output/error to redirect
      pipeToLog(poll_fds[0], m_stderr_pipe, logWidget);
      pipeToLog(poll_fds[1], m_stdout_pipe, logWidget);

      // There is a key to read
      handleKeyPress(poll_fds[2], logWidget);

      {
        std::lock_guard<std::mutex> p_lock(m_progress_info_mutex);
        progressWidget.update(m_progress_info);
      }

      // Update screen
      doupdate();

      // Wait for events
      if (poll(poll_fds, 4, 1000) < 0) {
        // poll may return with EINTR if a signal happened halfway
        m_exit_loop = (errno != EINTR);
      }
    } while (!m_exit_loop && !boost::this_thread::interruption_requested());
    m_log_text = logWidget.getText();
  }

public:
  /**
   * Constructor
   * @note
   *    Intercepts stdout and stderr and starts up the UI thread
   */
  Dashboard() {
    m_stderr_pipe = interceptFileDescriptor(STDERR_FILENO, &m_stderr_original);
    m_stdout_pipe = interceptFileDescriptor(STDOUT_FILENO, &m_stdout_original);
    int new_stderr_fd = dup(m_stderr_original);
    if (new_stderr_fd < 0) {
      throw std::system_error(errno, std::generic_category());
    }
    m_stderr = fdopen(new_stderr_fd, "w");
    m_ui_thread = Euclid::make_unique<boost::thread>(std::bind(&Dashboard::uiThread, this));
  }

  /**
   * Destructor
   * Notifies the UI thread to finish and clean up, and recover stdout and stderr
   */
  ~Dashboard() {
    if (m_ui_thread) {
      try {
        m_ui_thread->interrupt();
        if (m_ui_thread->joinable()) {
          m_ui_thread->join();
        }
      }
      catch (...) {
        // Ignore
      }
    }
    fclose(m_stderr);
    // Unneeded duplicates now
    close(m_stderr_original);
    close(m_stdout_original);
    close(m_stderr_pipe);
    close(m_stdout_pipe);
  }

  /**
   * Update the progress information
   */
  void update(const std::list<ProgressInfo>& info) {
    std::lock_guard<std::mutex> p_lock(m_progress_info_mutex);
    m_trigger_resize = (m_progress_info.size() != info.size()) || m_trigger_resize;
    m_progress_info = info;
  }
};

ProgressNCurses::ProgressNCurses() {
  m_dashboard = make_unique<Dashboard>();
}

ProgressNCurses::~ProgressNCurses() = default;

bool ProgressNCurses::isTerminalCapable() {
  return isatty(STDERR_FILENO);
}

void ProgressNCurses::handleMessage(const std::list<ProgressInfo>& info) {
  if (m_dashboard)
    m_dashboard->update(info);
}

void ProgressNCurses::handleMessage(const bool& done) {
  if (done && m_dashboard)
    m_dashboard.reset(nullptr);
}

} // end SourceXtractor
