/*
 * ProgressLogger.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSLOGGER_H
#define _SEMAIN_PROGRESSLOGGER_H

#include "SEMain/ProgressPrinter.h"
#include <ElementsKernel/Logging.h>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace SExtractor {

class ProgressLogger: public ProgressPrinter {
public:
  virtual ~ProgressLogger() = default;

  ProgressLogger(const boost::posix_time::time_duration &min_interval);

  void print(bool done = false) override;

protected:
  Elements::Logging m_logger;
  boost::posix_time::time_duration m_min_interval;
  boost::posix_time::ptime m_started, m_last_logged;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSLOGGER_H
