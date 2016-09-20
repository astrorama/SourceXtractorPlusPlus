/*
 * DetectionImage.h
 *
 *  Created on: Sep 13, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_
#define _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class DetectionFrame : public Property {
public:
  DetectionFrame(std::shared_ptr<DetectionImage> detection_image) : m_detection_image(detection_image) {
  }

  std::shared_ptr<DetectionImage> getDetectionImage() const {
    return m_detection_image;
  }

private:
  std::shared_ptr<DetectionImage> m_detection_image;
};

}

#endif /* _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_ */
