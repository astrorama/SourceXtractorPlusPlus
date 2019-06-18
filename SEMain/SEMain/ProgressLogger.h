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
#include <chrono>

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
  ProgressLogger(const std::chrono::steady_clock::duration& min_interval);

  void handleMessage(const std::map<std::string, Progress>& info) override;

  void handleMessage(const bool& done) override;

protected:
  Elements::Logging m_logger;
  std::chrono::steady_clock::duration m_min_interval;
  std::chrono::steady_clock::time_point m_started, m_last_logged;

  void print();
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSLOGGER_H
