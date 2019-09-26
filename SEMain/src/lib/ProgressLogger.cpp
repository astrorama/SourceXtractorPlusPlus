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
#include <iomanip>
#include <boost/date_time.hpp>
#include "SEMain/ProgressLogger.h"

namespace SExtractor {

ProgressLogger::ProgressLogger(const std::chrono::steady_clock::duration& min_interval) :
  m_logger{Elements::Logging::getLogger("Progress")}, m_min_interval{min_interval},
  m_started{std::chrono::steady_clock::now()},
  m_last_logged{m_started - m_min_interval}, m_done{false} {
}

void ProgressLogger::print() {
  auto now = std::chrono::steady_clock::now();

  if (now - m_last_logged > m_min_interval || m_done) {
    auto elapsed = now - m_started;
    m_last_logged = now;

    for (auto entry : m_progress_info) {
      // When there is no total, log an absolute count
      if (entry.m_total <= 0) {
        m_logger.info() << entry.m_label << ": " << entry.m_done;
      }
        // Otherwise, report progress
      else {
        float percent = (entry.m_done * 100.) / entry.m_total;
        m_logger.info() << entry.m_label << ": " << entry.m_done << " / " << entry.m_total
                        << " (" << std::fixed << std::setprecision(2) << percent << "%)";
      }
    }

    auto h = std::chrono::duration_cast<std::chrono::hours>(elapsed);
    auto m = std::chrono::duration_cast<std::chrono::minutes>(elapsed - h);
    auto s = std::chrono::duration_cast<std::chrono::seconds>(elapsed - h - m);
    m_logger.info() << "Elapsed: " << std::setfill('0')
                    << std::setw(2) << h.count() << ':'
                    << std::setw(2) << m.count() << ':'
                    << std::setw(2) << s.count();
  }
}

void ProgressLogger::handleMessage(const std::list<ProgressInfo>& info) {
  m_progress_info = info;
  print();
}

void ProgressLogger::handleMessage(const bool& done) {
  m_done = done;
  print();
}

} // end SExtractor
