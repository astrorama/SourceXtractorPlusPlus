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

  void handleMessage(const std::shared_ptr<SourceGroupInterface>&) override {
    ++m_counter;
    m_progress_listener.ping();
  }

private:
  ProgressListener &m_progress_listener;
  std::atomic_int &m_counter;
};

ProgressListener::ProgressListener(Elements::Logging &logger, boost::posix_time::time_duration min_interval) :
  m_logger(logger),
  m_min_interval{min_interval}, m_first{}, m_last{boost::posix_time::second_clock::local_time()},
  m_detected{0}, m_measured{0}, m_emitted{0},
  m_detection_listener{new ListenerCounter{*this, m_detected}},
  m_measurement_listener{new ListenerCounter{*this, m_measured}},
  m_emission_listener{new ListenerCounter{*this, m_emitted}} {
}

std::shared_ptr<ProgressListener::observer_t> &ProgressListener::getDetectionListener() {
  return m_detection_listener;
}

std::shared_ptr<ProgressListener::observer_t> &ProgressListener::getMeasurementListener() {
  return m_measurement_listener;
}

std::shared_ptr<ProgressListener::observer_t> &ProgressListener::getEmissionListener() {
  return m_emission_listener;
}

void ProgressListener::ping() {
  auto now = boost::posix_time::second_clock::local_time();

  if (now - m_last > m_min_interval) {
    if (m_first.is_not_a_date_time()) {
      m_first = now;
    }

    auto elapsed = now - m_first;
    float progress_measured = (m_measured * 100.) / m_detected;
    float progress_emitted = (m_emitted * 100.) / m_detected;

    m_logger.info() << "Detected: " << m_detected;
    m_logger.info() << "Measured: " << m_measured << " (" << std::fixed << std::setprecision(2) << progress_measured << "%)";
    m_logger.info() << "Emitted:  " << m_emitted << " (" << std::fixed << std::setprecision(2) << progress_emitted << "%)";
    m_logger.info() << "Elapsed: " << elapsed;

    m_last = now;
  }
}

} // end SExtractor
