/*
 * Progress.h
 *
 *  Created on: Sep 07, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESS_H_
#define _SEMAIN_PROGRESS_H_

#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEUtils/Observable.h"
#include <ElementsKernel/Logging.h>
#include <atomic>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace SExtractor {

class ProgressListener {
public:
  typedef Observer<std::shared_ptr<SourceGroupInterface>> observer_t;

  ~ProgressListener() = default;
  ProgressListener(Elements::Logging &logger, boost::posix_time::time_duration min_interval);

  std::shared_ptr<observer_t>& getDetectionListener(void);
  std::shared_ptr<observer_t>& getMeasurementListener(void);
  void ping(void);

private:
  Elements::Logging &m_logger;
  boost::posix_time::time_duration m_min_interval;
  boost::posix_time::ptime m_first, m_last;
  std::atomic_int m_detected, m_measured;

  std::shared_ptr<observer_t> m_detection_listener, m_measurement_listener;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESS_H_
