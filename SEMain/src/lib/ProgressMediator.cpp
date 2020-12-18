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
 * Progress.cpp
 *
 *  Created on: Sep 07, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEMain/ProgressMediator.h"

namespace SourceXtractor {

class ProgressMediator::ProgressCounter : public Observer<SegmentationProgress> {
public:
  ProgressCounter(ProgressMediator& progress_listener, SegmentationProgress& segmentation_progress,
                  std::mutex& mutex) :
    m_progress_listener(progress_listener), m_segmentation_progress(segmentation_progress),
    m_mutex(mutex) {
  }

  void handleMessage(const SegmentationProgress& progress) override {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_segmentation_progress = progress;
    }
    m_progress_listener.update();
  }

private:
  ProgressMediator& m_progress_listener;
  SegmentationProgress& m_segmentation_progress;
  std::mutex& m_mutex;
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
  m_segmentation_listener{std::make_shared<ProgressCounter>(*this, m_segmentation_progress, m_mutex)},
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
  this->ProgressObservable::notifyObservers(std::list<ProgressInfo>{
    {"Segmentation", m_segmentation_progress.position, m_segmentation_progress.total},
    {"Detected",     m_detected,                       -1},
    {"Deblended",    m_deblended,                      -1},
    {"Measured",     m_measured,                       m_deblended},
  });
}

void ProgressMediator::done() {
  std::lock_guard<std::mutex> guard(m_mutex);
  this->DoneObservable::notifyObservers(true);
}

} // end SourceXtractor
