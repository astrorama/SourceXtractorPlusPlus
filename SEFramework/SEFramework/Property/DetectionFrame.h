/*
 * DetectionImage.h
 *
 *  Created on: Sep 13, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_
#define _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_

#include "SEFramework/Frame/Frame.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

class DetectionFrame : public Property {
public:
  DetectionFrame(std::shared_ptr<DetectionImageFrame> detection_frame) : m_detection_frame(detection_frame) {
  }

  std::shared_ptr<DetectionImageFrame> getFrame() const {
    return m_detection_frame;
  }

private:
  std::shared_ptr<DetectionImageFrame> m_detection_frame;
};

}

#endif /* _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_ */
