/*
 * Progress.cpp
 *
 *  Created on: Sep 07, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <ctime>
#include <iomanip>
#include "SEMain/Progress.h"

namespace SExtractor {

class ListenerCounter : public ProgressListener::observer_t {
public:
  ListenerCounter(ProgressListener &progress_listener, std::atomic_int &counter) : m_progress_listener{
    progress_listener}, m_counter(counter) {}

  void handleMessage(const std::shared_ptr<SourceGroupInterface> &message) override {
    ++m_counter;
    m_progress_listener.ping();
  }

private:
  std::atomic_int &m_counter;
  ProgressListener &m_progress_listener;
};

ProgressListener::ProgressListener(Elements::Logging &logger, boost::posix_time::time_duration min_interval) :
  m_logger(logger),
  m_min_interval{min_interval}, m_first{}, m_last{boost::posix_time::second_clock::local_time()},
  m_detected{0}, m_measured{0},
  m_detection_listener{new ListenerCounter{*this, m_detected}},
  m_measurement_listener{new ListenerCounter{*this, m_measured}} {
}

std::shared_ptr<ProgressListener::observer_t> &ProgressListener::getDetectionListener() {
  return m_detection_listener;
}

std::shared_ptr<ProgressListener::observer_t> &ProgressListener::getMeasurementListener() {
  return m_measurement_listener;
}

void ProgressListener::ping() {
  auto now = boost::posix_time::second_clock::local_time();

  if (now - m_last > m_min_interval) {
    if (m_first.is_not_a_date_time()) {
      m_first = now;
    }

    auto elapsed = now - m_first;
    float progress = (m_measured * 100.) / m_detected;

    m_logger.info() << "Detected sources: " << m_detected;
    m_logger.info() << "Measured sources: " << m_measured << " (" << std::setprecision(2) << progress << "%)";
    m_logger.info() << "Elapsed: " << elapsed;

    m_last = now;
  }
}

} // end SExtractor
