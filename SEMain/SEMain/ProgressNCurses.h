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
 * ProgressNCurses.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSBAR_H
#define _SEMAIN_PROGRESSBAR_H

#include "SEMain/ProgressReporter.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/thread.hpp>
#include <vector>

namespace SExtractor {

/**
 * @class ProgressNCurses
 * This concrete implementation of ProgressReporter uses a progress bar
 * on the bottom of the terminal to report the progress. std::cerr is used, as std::cout could have been used
 * for the output catalog.
 * It uses a separate thread, and refreshes the content every second.
 * It relies on raw ANSI escape codes, instead of ncurses, to avoid introducing an extra dependency.
 * @see https://en.wikipedia.org/wiki/ANSI_escape_code
 */
class ProgressNCurses: public ProgressReporter {
public:
  virtual ~ProgressNCurses();

  ProgressNCurses();

  /**
   * This class intercepts the first call to update to decide on the size of the bottom
   * block, and start the thread.
   */
  void handleMessage(const std::list<ProgressInfo> & info) override;

  /**
   * Give the printing thread a chance to exit.
   */
  void handleMessage(const bool &done) override;

  /**
   * @return true if the attached terminal has the required capabilities to render the progress bar
   */
  static bool isTerminalCapable();

protected:
  class Dashboard;
  std::unique_ptr<Dashboard> m_dashboard;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSBAR_H
