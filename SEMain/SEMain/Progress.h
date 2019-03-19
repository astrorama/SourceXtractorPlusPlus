/*
 * Progress.h
 *
 *  Created on: Sep 07, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESS_H_
#define _SEMAIN_PROGRESS_H_

#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Pipeline/Segmentation.h"
#include "SEUtils/Observable.h"
#include <ElementsKernel/Logging.h>
#include <atomic>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace SExtractor {

class ProgressListener {
public:
  typedef Observer<SegmentationProgress> segmentation_observer_t;
  typedef Observer<std::shared_ptr<SourceInterface>> source_observer_t;
  typedef Observer<std::shared_ptr<SourceGroupInterface>> group_observer_t;

  ~ProgressListener() = default;
  ProgressListener(Elements::Logging &logger, boost::posix_time::time_duration min_interval);

  std::shared_ptr<segmentation_observer_t>& getSegmentationListener(void);
  std::shared_ptr<source_observer_t>& getDetectionListener(void);
  std::shared_ptr<group_observer_t>& getDeblendingListener(void);
  std::shared_ptr<group_observer_t>& getMeasurementListener(void);
  std::shared_ptr<group_observer_t>& getEmissionListener(void);
  void print(bool force = false);

private:
  Elements::Logging &m_logger;
  boost::posix_time::time_duration m_min_interval;
  boost::posix_time::ptime m_first, m_last;
  SegmentationProgress m_segmentation_progress;
  std::atomic_int m_detected, m_deblended, m_measured, m_emitted;

  std::shared_ptr<segmentation_observer_t> m_segmentation_listener;
  std::shared_ptr<source_observer_t> m_detection_listener;
  std::shared_ptr<group_observer_t> m_deblending_listener, m_measurement_listener, m_emission_listener;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESS_H_
