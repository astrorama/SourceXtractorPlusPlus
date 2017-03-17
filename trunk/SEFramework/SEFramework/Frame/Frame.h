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
        std::shared_ptr<CoordinateSystem> coordinate_system)
          : m_image(detection_image),
            m_weight_image(weight_image),
            m_coordinate_system(coordinate_system),

            m_background_level_available(false),
            m_background_level(0),

            m_background_rms_available(false),
            m_background_rms(0),

            m_detection_threshold_available(false),
            m_detection_threshold(0)
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

  typename T::PixelType getBackgroundRMS() const {
    if (!m_background_rms_available) {
      const_cast<Frame<T>*>(this)->computeBackgroundRMS();
    }
    return m_background_rms;
  }

  typename T::PixelType getBackgroundLevel() const {
    if (!m_background_level_available) {
      const_cast<Frame<T>*>(this)->computeBackgroundLevel();
    }
    return m_background_level;
  }

  typename T::PixelType getDetectionThreshold() const {
    if (!m_detection_threshold_available) {
      const_cast<Frame<T>*>(this)->computeDetectionThreshold();
    }
    return m_detection_threshold;
  }

  void applyFilter(const ImageProcessing<typename T::PixelType>& image_processing) {
    m_filtered_image = image_processing.processImage(m_image);
  }

  void setDetectionThreshold(typename T::PixelType detection_threshold) {
    m_detection_threshold = detection_threshold;
    m_detection_threshold_available = true;
  }

  void setBackgroundLevel(typename T::PixelType background_level) {
    m_background_level = background_level;
    m_background_level_available = true;
  }


private:
  void computeBackgroundRMS();
  void computeBackgroundLevel();
  void computeDetectionThreshold();

  std::shared_ptr<T> m_image;
  std::shared_ptr<T> m_filtered_image;
  std::shared_ptr<WeightImage> m_weight_image;
  std::shared_ptr<CoordinateSystem> m_coordinate_system;

  bool m_background_level_available;
  typename T::PixelType m_background_level;

  bool m_background_rms_available;
  typename T::PixelType m_background_rms;

  bool m_detection_threshold_available;
  typename T::PixelType m_detection_threshold;
};

using DetectionImageFrame = Frame<DetectionImage>;
using MeasurementImageFrame = Frame<MeasurementImage>;

template<typename T>
void Frame<T>::computeBackgroundRMS() {
  // Note: We compute the RMS by only taking into consideration pixels
  // below the background level.
  auto background_level = getBackgroundLevel();
  double variance = 0;
  int pixels = 0;
  for (int y=0; y < m_image->getHeight(); y++) {
    for (int x=0; x < m_image->getWidth(); x++) {
      auto value = m_image->getValue(x, y);
      if (value < background_level) {
        auto diff = value - background_level;
        variance += diff * diff;
        pixels++;
      }
    }
  }
  if (pixels > 0) {
    variance /= pixels;
  }

  m_background_rms = sqrt(variance);
  m_background_rms_available = true;
}

template<typename T>
void Frame<T>::computeBackgroundLevel() {
  if (m_image != nullptr) {
    auto image_copy = std::make_shared<VectorImage<typename T::PixelType>>(*m_image);
    std::sort(image_copy->getData().begin(), image_copy->getData().end());
    m_background_level = image_copy->getData()[image_copy->getData().size()/2];
    m_background_level_available = true;
  }
}

template<typename T>
void Frame<T>::computeDetectionThreshold() {
  m_detection_threshold = 1.5 * getBackgroundRMS();
  m_detection_threshold_available = true;
}

}

#endif /* _SEFRAMEWORK_FRAME_FRAME_H_ */
