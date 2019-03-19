/*
 * Progress.cpp
 *
 *  Created on: Sep 07, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <ctime>
#include <iomanip>
#include "SEMain/ProgressListener.h"

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

ProgressListener::ProgressListener(const std::shared_ptr<ProgressPrinter> &printer) :
  m_printer{printer},
  m_detected{0}, m_deblended{0}, m_measured{0}, m_emitted{0},
  m_segmentation_listener{std::make_shared<ProgressCounter>(*this, m_segmentation_progress)},
  m_detection_listener{std::make_shared<SourceCounter>(*this, m_detected)},
  m_deblending_listener{std::make_shared<GroupCounter>(*this, m_deblended)},
  m_measurement_listener{std::make_shared<GroupCounter>(*this, m_measured)},
  m_emission_listener{std::make_shared<GroupCounter>(*this, m_emitted)} {
}

std::shared_ptr<ProgressListener::segmentation_observer_t>& ProgressListener::getSegmentationObserver() {
  return m_segmentation_listener;
}

std::shared_ptr<ProgressListener::source_observer_t>& ProgressListener::getDetectionObserver() {
  return m_detection_listener;
}

std::shared_ptr<ProgressListener::group_observer_t>& ProgressListener::getDeblendingObserver() {
  return m_deblending_listener;
}

std::shared_ptr<ProgressListener::group_observer_t>& ProgressListener::getMeasurementObserver() {
  return m_measurement_listener;
}

std::shared_ptr<ProgressListener::group_observer_t>& ProgressListener::getEmissionObserver() {
  return m_emission_listener;
}

void ProgressListener::print(void) {
  m_printer->update("Detected", m_detected);
  m_printer->update("Deblended", m_deblended);
  m_printer->update("Segmentation", m_segmentation_progress.position, m_segmentation_progress.total);
  m_printer->update("Measured", m_measured, m_deblended);
  m_printer->update("Emitted", m_emitted, m_deblended);
  m_printer->print();
}

} // end SExtractor
