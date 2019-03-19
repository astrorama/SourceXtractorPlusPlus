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

class ProgressCounter: public Observer<SegmentationProgress> {
public:
  ProgressCounter(ProgressListener& progress_listener, SegmentationProgress &segmentation_progress):
    m_progress_listener{progress_listener}, m_segmentation_progress{segmentation_progress} {
  }

  void handleMessage(const SegmentationProgress &progress) override {
    m_segmentation_progress = progress;
    m_progress_listener.print();
  }

private:
  ProgressListener& m_progress_listener;
  SegmentationProgress& m_segmentation_progress;
};

class SourceCounter : public Observer<std::shared_ptr<SourceInterface>> {
public:
  SourceCounter(ProgressListener& progress_listener, std::atomic_int& counter) :
    m_progress_listener{progress_listener}, m_counter(counter) {}

  void handleMessage(const std::shared_ptr<SourceInterface>&) override {
    ++m_counter;
    m_progress_listener.print();
  }

private:
  ProgressListener& m_progress_listener;
  std::atomic_int& m_counter;
};

class GroupCounter : public Observer<std::shared_ptr<SourceGroupInterface>> {
public:
  GroupCounter(ProgressListener& progress_listener, std::atomic_int& counter) :
    m_progress_listener{progress_listener}, m_counter(counter) {}

  void handleMessage(const std::shared_ptr<SourceGroupInterface>& group) override {
    m_counter += group->size();
    m_progress_listener.print();
  }

private:
  ProgressListener& m_progress_listener;
  std::atomic_int& m_counter;
};

ProgressListener::ProgressListener(Elements::Logging& logger, boost::posix_time::time_duration min_interval) :
  m_logger(logger),
  m_min_interval{min_interval}, m_first{}, m_last{boost::posix_time::second_clock::local_time()},
  m_detected{0}, m_deblended{0}, m_measured{0}, m_emitted{0},
  m_segmentation_listener{std::make_shared<ProgressCounter>(*this, m_segmentation_progress)},
  m_detection_listener{std::make_shared<SourceCounter>(*this, m_detected)},
  m_deblending_listener{std::make_shared<GroupCounter>(*this, m_deblended)},
  m_measurement_listener{std::make_shared<GroupCounter>(*this, m_measured)},
  m_emission_listener{std::make_shared<GroupCounter>(*this, m_emitted)} {
}

std::shared_ptr<ProgressListener::segmentation_observer_t>& ProgressListener::getSegmentationListener() {
  return m_segmentation_listener;
}

std::shared_ptr<ProgressListener::source_observer_t>& ProgressListener::getDetectionListener() {
  return m_detection_listener;
}

std::shared_ptr<ProgressListener::group_observer_t>& ProgressListener::getDeblendingListener() {
  return m_deblending_listener;
}

std::shared_ptr<ProgressListener::group_observer_t>& ProgressListener::getMeasurementListener() {
  return m_measurement_listener;
}

std::shared_ptr<ProgressListener::group_observer_t>& ProgressListener::getEmissionListener() {
  return m_emission_listener;
}

void ProgressListener::print(bool force) {
  auto now = boost::posix_time::second_clock::local_time();

  if (now - m_last > m_min_interval || force) {
    if (m_first.is_not_a_date_time()) {
      m_first = now;
    }

    auto elapsed = now - m_first;
    float progress_segmentation = (m_segmentation_progress.position * 100.) / m_segmentation_progress.total;
    float progress_measured = (m_measured * 100.) / m_deblended;
    float progress_emitted = (m_emitted * 100.) / m_deblended;

    m_logger.info() << m_segmentation_progress.position << " / " << m_segmentation_progress.total
                    << " " << m_segmentation_progress.unit
                    << " (" << std::fixed << std::setprecision(2) << progress_segmentation << "%)";
    m_logger.info() << "Detected: " << m_detected;
    m_logger.info() << "Deblended: " << m_deblended;
    m_logger.info() << "Measured: " << m_measured
                    << " (" << std::fixed << std::setprecision(2) << progress_measured << "%)";
    m_logger.info() << "Emitted:  " << m_emitted
                    << " (" << std::fixed << std::setprecision(2) << progress_emitted << "%)";
    m_logger.info() << "Elapsed: " << elapsed;

    m_last = now;
  }
}

} // end SExtractor
