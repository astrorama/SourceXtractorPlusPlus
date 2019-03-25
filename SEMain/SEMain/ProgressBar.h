/*
 * ProgressBar.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSBAR_H
#define _SEMAIN_PROGRESSBAR_H

#include "SEMain/ProgressPrinter.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/thread.hpp>
#include <vector>

namespace SExtractor {

class ProgressBar: public ProgressPrinter {
private:
  ProgressBar();

public:
  virtual ~ProgressBar();

  static std::shared_ptr<ProgressBar> getInstance();

  void setElements(const std::vector<std::string>& entries);

  void restoreTerminal();

  void updateTerminal();

  void print() override;

  void done() override;

protected:
  boost::posix_time::ptime m_started;
  size_t m_progress_row, m_value_position, m_bar_width;
  std::unique_ptr<boost::thread> m_progress_thread;

  static void printThread();
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSBAR_H
