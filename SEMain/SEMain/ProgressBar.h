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
  virtual ~ProgressBar() = default;
  ProgressBar(std::ostream &);

  void print(bool done = false) override;

protected:
  std::ostream &m_out;
  boost::posix_time::ptime m_started;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSBAR_H
