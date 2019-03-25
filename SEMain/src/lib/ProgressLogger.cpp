#include <iomanip>
#include <boost/date_time.hpp>
#include "SEMain/ProgressLogger.h"

namespace SExtractor {

ProgressLogger::ProgressLogger(const boost::posix_time::time_duration& min_interval) :
  m_logger{Elements::Logging::getLogger("Progress")}, m_min_interval{min_interval},
  m_started{boost::posix_time::second_clock::local_time()},
  m_last_logged{boost::posix_time::second_clock::local_time() - m_min_interval} {
}

void ProgressLogger::print() {
  auto now = boost::posix_time::second_clock::local_time();

  if (now - m_last_logged > m_min_interval || m_done) {
    auto elapsed = now - m_started;
    m_last_logged = now;

    for (auto entry : m_progress_info) {
      // When there is no total, log an absolute count
      if (entry.second.second < 0) {
        m_logger.info() << entry.first << ": " << entry.second.first;
      }
        // Otherwise, report progress
      else {
        float percent = (entry.second.first * 100.) / entry.second.second;
        m_logger.info() << entry.first << ": " << entry.second.first << " / " << entry.second.second
                        << " (" << std::fixed << std::setprecision(2) << percent << "%)";
      }
    }

    m_logger.info() << "Elapsed: " << elapsed;
  }
}

void ProgressLogger::update(const std::map<std::string, std::pair<int, int>>& info) {
  this->ProgressReporter::update(info);
  print();
}

void ProgressLogger::done() {
  this->ProgressReporter::done();
  print();
}

} // end SExtractor
