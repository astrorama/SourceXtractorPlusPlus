/*
 * DetectionThreshold.h
 *
 *  Created on: Mar 1, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_DETECTIONTHRESHOLD_H_
#define _SEIMPLEMENTATION_PROPERTY_DETECTIONTHRESHOLD_H_

namespace SExtractor {

class DetectionThreshold : public Property {

public:

  DetectionThreshold(SeFloat detection_threshold)
      : m_detection_threshold(detection_threshold) {
  }

  virtual ~DetectionThreshold() = default;

  SeFloat getDetectionThreshold() const {
    return m_detection_threshold;
  }

private:

  SeFloat m_detection_threshold;

};

}

#endif /* _SEIMPLEMENTATION_PROPERTY_DETECTIONTHRESHOLD_H_ */
