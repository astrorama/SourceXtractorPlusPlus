/**
 * @file SEImplementation/DetectionFrameSourceStamp.h
 * @date 06/03/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAMESOURCESTAMP_H
#define _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAMESOURCESTAMP_H

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class DetectionFrameSourceStamp
 * @brief
 *
 */
class DetectionFrameSourceStamp : public Property {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStamp() = default;

  DetectionFrameSourceStamp(std::shared_ptr<DetectionImage> stamp) : m_stamp(stamp) {}

  const DetectionImage& getStamp() const {
    return *m_stamp;
  }

private:

  std::shared_ptr<DetectionImage> m_stamp;

}; /* End of DetectionFrameSourceStamp class */


} /* namespace SEImplementation */


#endif
