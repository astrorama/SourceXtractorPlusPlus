/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file SEImplementation/DetectionFrameSourceStamp.h
 * @date 06/03/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAMESOURCESTAMP_H
#define _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAMESOURCESTAMP_H

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {

/**
 * @class DetectionFrameSourceStamp
 * @brief A copy of the rectangular region of the detection image just large enough to include the whole Source
 */

class DetectionFrameSourceStamp : public Property {

public:

  using DetectionVectorImage = VectorImage<DetectionImage::PixelType>;
  using WeightVectorImage = VectorImage<WeightImage::PixelType>;

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStamp() = default;

  DetectionFrameSourceStamp(std::shared_ptr<DetectionVectorImage> stamp,
                            std::shared_ptr<DetectionVectorImage> filtered_stamp,
                            std::shared_ptr<DetectionVectorImage> thresholded_stamp,
                            PixelCoordinate top_left,
                            std::shared_ptr<WeightVectorImage> variance_stamp,
                            std::shared_ptr<DetectionVectorImage> threshold_map_stamp) :
    m_stamp(stamp), m_filtered_stamp(filtered_stamp), m_thresholded_stamp(thresholded_stamp),
    m_threshold_map_stamp(threshold_map_stamp), m_variance_stamp(variance_stamp),
    m_top_left(top_left) {}

  // Returns the stamp image
  const DetectionVectorImage& getStamp() const {
    return *m_stamp;
  }

  // Returns the filtered stamp image
  const DetectionVectorImage& getFilteredStamp() const {
    return *m_filtered_stamp;
  }

  // Returns the filtered and thresholded stamp image
  const DetectionVectorImage& getThresholdedStamp() const {
    return *m_thresholded_stamp;
  }

  // Returns the threshold map stamp
  const DetectionVectorImage& getThresholdMapStamp() const {
    return *m_threshold_map_stamp;
  }

  // Returns the stamp's associated weight image
  const WeightVectorImage& getVarianceStamp() const {
    return *m_variance_stamp;
  }

  PixelCoordinate getTopLeft() const {
    return m_top_left;
  }

private:
  std::shared_ptr<DetectionVectorImage> m_stamp, m_filtered_stamp;
  std::shared_ptr<DetectionVectorImage> m_thresholded_stamp, m_threshold_map_stamp;
  std::shared_ptr<WeightVectorImage> m_variance_stamp;
  PixelCoordinate m_top_left;

}; /* End of DetectionFrameSourceStamp class */


} /* namespace SourceXtractor */


#endif
