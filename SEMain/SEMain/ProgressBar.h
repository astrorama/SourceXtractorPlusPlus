/*
 * ProgressBar.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSBAR_H
#define _SEMAIN_PROGRESSBAR_H

#include <boost/date_time/posix_time/ptime.hpp>
#include "SEMain/ProgressPrinter.h"

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

  void print(bool done = false) override;

protected:
  boost::posix_time::ptime m_started;
  size_t m_progress_row, m_value_position, m_bar_width;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSBAR_H
