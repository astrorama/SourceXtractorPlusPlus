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

namespace SourceXtractor {

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
  explicit ProgressLogger(const std::chrono::steady_clock::duration& min_interval);

  void handleMessage(const std::list<ProgressInfo>& info) override;

  void handleMessage(const bool& done) override;

protected:
  Elements::Logging m_logger;
  std::chrono::steady_clock::duration m_min_interval;
  std::chrono::steady_clock::time_point m_started, m_last_logged;
  std::list<ProgressInfo> m_progress_info;
  std::atomic_bool m_done;

  void print();
};

} // end SourceXtractor

#endif // _SEMAIN_PROGRESSLOGGER_H
