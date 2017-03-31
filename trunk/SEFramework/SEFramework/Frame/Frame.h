/*
 * Frame.h
 *
 *  Created on: Mar 13, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_FRAME_FRAME_H_
#define _SEFRAMEWORK_FRAME_FRAME_H_

#include <algorithm>

#include "SEUtils/Types.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/SubtractImage.h"
#include "SEFramework/Image/ImageProcessing.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

template<typename T>
class Frame {

public:

  Frame(std::shared_ptr<T> detection_image,
        std::shared_ptr<WeightImage> weight_image,
        bool is_weight_absolute,
        std::shared_ptr<CoordinateSystem> coordinate_system)
          : m_image(detection_image),
            m_weight_image(weight_image),
            m_coordinate_system(coordinate_system),

            m_background_level(0),
            m_background_rms(0),
            m_detection_threshold(0),

            m_is_weight_absolute(is_weight_absolute)
            {}

  std::shared_ptr<T> getOriginalImage() const {
    return m_image;
  }

  // Get the image with background subtraction
  std::shared_ptr<T> getSubtractedImage() const {
    return std::make_shared<SubtractImage<typename T::PixelType>>(getOriginalImage(), getBackgroundLevel());
  }

  std::shared_ptr<T> getFilteredImage() const {
    if (m_filtered_image !=  nullptr) {
      return m_filtered_image;
    } else {
      return getSubtractedImage();
    }
  }

  std::shared_ptr<T> getThresholdedImage() const {
    return std::make_shared<SubtractImage<typename T::PixelType>>(getFilteredImage(), getDetectionThreshold());
  }


  std::shared_ptr<CoordinateSystem> getCoordinateSystem() const {
    return m_coordinate_system;
  }

  std::shared_ptr<WeightImage> getWeightImage() const {
    return m_weight_image;
  }

  bool isWeightAbsolute() const {
    return m_is_weight_absolute;
  }

  typename T::PixelType getBackgroundRMS() const {
    return m_background_rms;
  }

  typename T::PixelType getBackgroundLevel() const {
    return m_background_level;
  }

  typename T::PixelType getDetectionThreshold() const {
    return m_detection_threshold;
  }

  void applyFilter(const ImageProcessing<typename T::PixelType>& image_processing) {
    m_filtered_image = image_processing.processImage(getSubtractedImage());
  }

  void setDetectionThreshold(typename T::PixelType detection_threshold) {
    m_detection_threshold = detection_threshold;
  }

  void setBackgroundRMS(typename T::PixelType background_rms) {
    m_background_rms = background_rms;
  }

  void setBackgroundLevel(typename T::PixelType background_level) {
    m_background_level = background_level;
  }


private:
  std::shared_ptr<T> m_image;
  std::shared_ptr<T> m_filtered_image;
  std::shared_ptr<WeightImage> m_weight_image;
  std::shared_ptr<CoordinateSystem> m_coordinate_system;

  typename T::PixelType m_background_level;
  typename T::PixelType m_background_rms;
  typename T::PixelType m_detection_threshold;

  bool m_is_weight_absolute;
};

using DetectionImageFrame = Frame<DetectionImage>;
using MeasurementImageFrame = Frame<MeasurementImage>;

}

#endif /* _SEFRAMEWORK_FRAME_FRAME_H_ */
