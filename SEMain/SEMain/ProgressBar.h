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
public:
  virtual ~ProgressBar();
  ProgressBar(std::ostream &, const std::initializer_list<std::string> &entries);

  void print(bool done = false) override;

protected:
  std::ostream &m_out;
  boost::posix_time::ptime m_started;
  size_t m_progress_row, m_value_position, m_bar_width;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSBAR_H
