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
 * ProgressMediator.h
 *
 *  Created on: Sep 07, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSLISTENER_H
#define _SEMAIN_PROGRESSLISTENER_H

#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Pipeline/Segmentation.h"
#include "SEUtils/Observable.h"
#include <atomic>
#include <mutex>

namespace SourceXtractor {

struct ProgressInfo {
  std::string m_label;
  int m_done, m_total;

  ProgressInfo(const std::string &name, int done, int total): m_label(name), m_done(done), m_total(total) {}
};

typedef Observable<std::list<ProgressInfo>> ProgressObservable;
typedef Observable<bool> DoneObservable;

/**
 * @class ProgressMediator
 * This class provides concrete observers for different steps of SourceXtractor,
 * so it can keep count of the progress.
 * It will wrap up this information and pass it along to an agnostic ProgressReporter.
 */
class ProgressMediator: public ProgressObservable, public DoneObservable {
public:
  typedef Observer<SegmentationProgress> segmentation_observer_t;
  typedef Observer<SourceInterface>      source_observer_t;
  typedef Observer<SourceGroupInterface> group_observer_t;

  ~ProgressMediator() = default;

  /**
   * Constructor
   */
  ProgressMediator();

  /**
   * @return An observer for the segmentation stage.
   */
  std::shared_ptr<segmentation_observer_t>& getSegmentationObserver(void);

  /**
   * @return An observer for the detection stage.
   */
  std::shared_ptr<source_observer_t>& getDetectionObserver(void);

  /**
   * @return An observer for the deblending stage.
   */
  std::shared_ptr<group_observer_t>& getDeblendingObserver(void);

  /**
   * @return An observer for the measurement stage.
   */
  std::shared_ptr<group_observer_t>& getMeasurementObserver(void);

  /**
   * Notify that the process is completely done
   */
  void done();

  /**
   * Trigger a notification
   */
  void update(void);

private:
  SegmentationProgress m_segmentation_progress;
  std::atomic_int m_detected, m_deblended, m_measured;

  std::shared_ptr<segmentation_observer_t> m_segmentation_listener;
  std::shared_ptr<source_observer_t> m_detection_listener;
  std::shared_ptr<group_observer_t> m_deblending_listener, m_measurement_listener;

  // Mediator serializes the notifications, so the observers do not need to worry about
  // being called from multiple threads
  std::mutex m_mutex;

  // This is a set of internal classes that implement the Observer pattern for different stages.
  class ProgressCounter;
  class SourceCounter;
  class GroupCounter;
};

} // end SourceXtractor

#endif // _SEMAIN_PROGRESSLISTENER_H
