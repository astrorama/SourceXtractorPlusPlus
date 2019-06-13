#include <iomanip>
#include <boost/date_time.hpp>
#include "SEMain/ProgressLogger.h"

namespace SExtractor {

ProgressLogger::ProgressLogger(const std::chrono::steady_clock::duration& min_interval) :
  m_logger{Elements::Logging::getLogger("Progress")}, m_min_interval{min_interval},
  m_started{std::chrono::steady_clock::now()},
  m_last_logged{m_started - m_min_interval} {
}

void ProgressLogger::print() {
  auto now = std::chrono::steady_clock::now();

  if (now - m_last_logged > m_min_interval || m_done) {
    auto elapsed = now - m_started;
    m_last_logged = now;

    for (auto entry : m_progress_info) {
      // When there is no total, log an absolute count
      if (entry.second.second <= 0) {
        m_logger.info() << entry.first << ": " << entry.second.first;
      }
        // Otherwise, report progress
      else {
        float percent = (entry.second.first * 100.) / entry.second.second;
        m_logger.info() << entry.first << ": " << entry.second.first << " / " << entry.second.second
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

void ProgressLogger::handleMessage(const std::map<std::string, std::pair<int, int>>& info) {
  this->ProgressReporter::handleMessage(info);
  print();
}

void ProgressLogger::handleMessage(const bool& done) {
  this->ProgressReporter::handleMessage(done);
  print();
}

} // end SExtractor
