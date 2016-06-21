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
 * @brief A copy of the rectangular region of the detection image just large enough to include the whole Source
 */

class DetectionFrameSourceStamp : public Property {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStamp() = default;

  DetectionFrameSourceStamp(std::shared_ptr<DetectionImage> stamp) : m_stamp(stamp) {}

  // Returns the stamp image
  const DetectionImage& getStamp() const {
    return *m_stamp;
  }

private:
  std::shared_ptr<DetectionImage> m_stamp;

}; /* End of DetectionFrameSourceStamp class */


} /* namespace SExtractor */


#endif
