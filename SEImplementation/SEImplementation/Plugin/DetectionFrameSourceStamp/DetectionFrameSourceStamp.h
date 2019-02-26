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

  DetectionFrameSourceStamp(std::shared_ptr<DetectionImage> stamp,
      std::shared_ptr<DetectionImage> thresholded_stamp, PixelCoordinate top_left,
      std::shared_ptr<WeightImage> variance_stamp) :
        m_stamp(stamp), m_thresholded_stamp(thresholded_stamp),
        m_variance_stamp(variance_stamp), m_top_left(top_left) {}

  // Returns the stamp image
  const DetectionImage& getStamp() const {
    return *m_stamp;
  }

  const DetectionImage& getThresholdedStamp() const {
    return *m_thresholded_stamp;
  }

  // Returns the stamp's associated weight image
  const DetectionImage& getVarianceStamp() const {
    return *m_variance_stamp;
  }

  PixelCoordinate getTopLeft() const {
    return m_top_left;
  }

private:
  std::shared_ptr<DetectionImage> m_stamp, m_thresholded_stamp;
  std::shared_ptr<WeightImage> m_variance_stamp;
  PixelCoordinate m_top_left;

}; /* End of DetectionFrameSourceStamp class */


} /* namespace SExtractor */


#endif
