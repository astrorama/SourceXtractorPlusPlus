/*
 * Progress.cpp
 *
 *  Created on: Sep 07, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <ctime>
#include <iomanip>
#include "SEMain/ProgressMediator.h"

namespace SExtractor {

class ProgressMediator::ProgressCounter : public Observer<SegmentationProgress> {
public:
  ProgressCounter(ProgressMediator& progress_listener, SegmentationProgress& segmentation_progress) :
    m_progress_listener(progress_listener), m_segmentation_progress(segmentation_progress) {
  }

  void handleMessage(const SegmentationProgress& progress) override {
    m_segmentation_progress = progress;
    m_progress_listener.update();
  }

private:
  ProgressMediator& m_progress_listener;
  SegmentationProgress& m_segmentation_progress;
};

class ProgressMediator::SourceCounter : public Observer<std::shared_ptr<SourceInterface>> {
public:
  SourceCounter(ProgressMediator& progress_listener, std::atomic_int& counter) :
    m_progress_listener(progress_listener), m_counter(counter) {}

  void handleMessage(const std::shared_ptr<SourceInterface>&) override {
    ++m_counter;
    m_progress_listener.update();
  }

private:
  ProgressMediator& m_progress_listener;
  std::atomic_int& m_counter;
};

class ProgressMediator::GroupCounter : public Observer<std::shared_ptr<SourceGroupInterface>> {
public:
  GroupCounter(ProgressMediator& progress_listener, std::atomic_int& counter) :
    m_progress_listener(progress_listener), m_counter(counter) {}

  void handleMessage(const std::shared_ptr<SourceGroupInterface>& group) override {
    m_counter += group->size();
    m_progress_listener.update();
  }

private:
  ProgressMediator& m_progress_listener;
  std::atomic_int& m_counter;
};

ProgressMediator::ProgressMediator() :
  m_segmentation_progress{0, 0}, m_detected{0}, m_deblended{0}, m_measured{0},
  m_segmentation_listener{std::make_shared<ProgressCounter>(*this, m_segmentation_progress)},
  m_detection_listener{std::make_shared<SourceCounter>(*this, m_detected)},
  m_deblending_listener{std::make_shared<GroupCounter>(*this, m_deblended)},
  m_measurement_listener{std::make_shared<GroupCounter>(*this, m_measured)} {
}

std::shared_ptr<ProgressMediator::segmentation_observer_t>& ProgressMediator::getSegmentationObserver() {
  return m_segmentation_listener;
}

std::shared_ptr<ProgressMediator::source_observer_t>& ProgressMediator::getDetectionObserver() {
  return m_detection_listener;
}

std::shared_ptr<ProgressMediator::group_observer_t>& ProgressMediator::getDeblendingObserver() {
  return m_deblending_listener;
}

std::shared_ptr<ProgressMediator::group_observer_t>& ProgressMediator::getMeasurementObserver() {
  return m_measurement_listener;
}

void ProgressMediator::update(void) {
  std::lock_guard<std::mutex> guard(m_mutex);
  this->ProgressObservable::notifyObservers(std::map<std::string, std::pair<int, int>>{
    {"Detected",     {m_detected,                       -1}},
    {"Deblended",    {m_deblended,                      -1}},
    {"Measured",     {m_measured,                       m_deblended}},
    {"Segmentation", {m_segmentation_progress.position, m_segmentation_progress.total}}
  });
}

void ProgressMediator::done() {
  std::lock_guard<std::mutex> guard(m_mutex);
  this->DoneObservable::notifyObservers(true);
}

} // end SExtractor
