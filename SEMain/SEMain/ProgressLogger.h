/*
 * ProgressLogger.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSLOGGER_H
#define _SEMAIN_PROGRESSLOGGER_H

#include "SEMain/ProgressReporter.h"
#include <ElementsKernel/Logging.h>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace SExtractor {

/**
 * @class ProgressLogger
 * This concrete implementation of ProgressReporter relies on Element's logging.
 * It only logs the progress if a minimum interval of time has passed since the las update.
 */
class ProgressLogger: public ProgressReporter {
public:
  virtual ~ProgressLogger() = default;

  /**
   * Constructor
   * @param min_interval
   *    Do not log an update if less than `min_interval` has elapsed since the last logged update.
   */
  ProgressLogger(const boost::posix_time::time_duration& min_interval);

  void update(const std::map<std::string, std::pair<int, int>> & info) override;
  void done() override;

protected:
  Elements::Logging m_logger;
  boost::posix_time::time_duration m_min_interval;
  boost::posix_time::ptime m_started, m_last_logged;

  void print();
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSLOGGER_H
