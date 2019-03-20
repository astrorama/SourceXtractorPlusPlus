/*
 * ProgressListener.h
 *
 *  Created on: Sep 07, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSLISTENER_H
#define _SEMAIN_PROGRESSLISTENER_H

#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Pipeline/Segmentation.h"
#include "SEMain/ProgressPrinter.h"
#include "SEUtils/Observable.h"
#include <atomic>

namespace SExtractor {

class ProgressListener {
public:
  typedef Observer<SegmentationProgress> segmentation_observer_t;
  typedef Observer<std::shared_ptr<SourceInterface>> source_observer_t;
  typedef Observer<std::shared_ptr<SourceGroupInterface>> group_observer_t;

  ~ProgressListener() = default;
  ProgressListener(const std::shared_ptr<ProgressPrinter> &printer);

  std::shared_ptr<segmentation_observer_t>& getSegmentationObserver(void);
  std::shared_ptr<source_observer_t>& getDetectionObserver(void);
  std::shared_ptr<group_observer_t>& getDeblendingObserver(void);
  std::shared_ptr<group_observer_t>& getMeasurementObserver(void);
  void print(void);

private:
  std::shared_ptr<ProgressPrinter> m_printer;
  SegmentationProgress m_segmentation_progress;
  std::atomic_int m_detected, m_deblended, m_measured;

  std::shared_ptr<segmentation_observer_t> m_segmentation_listener;
  std::shared_ptr<source_observer_t> m_detection_listener;
  std::shared_ptr<group_observer_t> m_deblending_listener, m_measurement_listener;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSLISTENER_H
