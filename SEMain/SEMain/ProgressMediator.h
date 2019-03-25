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
#include "SEMain/ProgressReporter.h"
#include "SEUtils/Observable.h"
#include <atomic>

namespace SExtractor {

/**
 * @class ProgressMediator
 * This class provides concrete observers for different steps of SExtractor,
 * so it can keep count of the progress.
 * It will wrap up this information and pass it along to an agnostic ProgressReporter.
 */
class ProgressMediator {
public:
  typedef Observer<SegmentationProgress> segmentation_observer_t;
  typedef Observer<std::shared_ptr<SourceInterface>> source_observer_t;
  typedef Observer<std::shared_ptr<SourceGroupInterface>> group_observer_t;

  ~ProgressMediator() = default;

  /**
   * Constructor
   * @param reporter
   *    The ProgressReporter to notify.
   */
  ProgressMediator(const std::shared_ptr<ProgressReporter> &reporter);

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

private:
  std::shared_ptr<ProgressReporter> m_reporter;
  SegmentationProgress m_segmentation_progress;
  std::atomic_int m_detected, m_deblended, m_measured;

  std::shared_ptr<segmentation_observer_t> m_segmentation_listener;
  std::shared_ptr<source_observer_t> m_detection_listener;
  std::shared_ptr<group_observer_t> m_deblending_listener, m_measurement_listener;

  /**
   * Trigger a notification
   */
  void update(void);

  // This is a set of internal classes that implement the Observer pattern for different stages.
  class ProgressCounter;
  class SourceCounter;
  class GroupCounter;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSLISTENER_H
