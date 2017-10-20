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
#include "SEFramework/Image/MultiplyImage.h"
#include "SEFramework/Image/ImageProcessing.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

template<typename T>
class Frame {

public:

  Frame(std::shared_ptr<T> detection_image,
        std::shared_ptr<WeightImage> weight_image,
        bool is_weight_absolute,
        WeightImage::PixelType weight_threshold,
        std::shared_ptr<CoordinateSystem> coordinate_system,
        SeFloat gain, SeFloat saturation)
          : m_image(detection_image),
            m_weight_image(weight_image),
            m_coordinate_system(coordinate_system),

            //m_detection_threshold(0),

            m_is_weight_absolute(is_weight_absolute),
            m_weight_threshold(weight_threshold),
            m_gain(gain),
            m_saturation(saturation)
            {}

  Frame(std::shared_ptr<T> detection_image,
        std::shared_ptr<CoordinateSystem> coordinate_system = nullptr)
          : m_image(detection_image),
            m_weight_image(nullptr),
            m_coordinate_system(coordinate_system),

            //m_detection_threshold(0),

            m_is_weight_absolute(false),
            m_weight_threshold(0),
            m_gain(0),
            m_saturation(0)
            {}

  std::shared_ptr<T> getOriginalImage() const {
    return m_image;
  }

  // Get the image with background subtraction
  std::shared_ptr<T> getSubtractedImage() const {
    return SubtractImage<typename T::PixelType>::create(getOriginalImage(), getBackgroundLevelMap());
  }

  std::shared_ptr<T> getFilteredImage() const {
    if (m_filtered_image !=  nullptr) {
      return m_filtered_image;
    } else {
      return getSubtractedImage();
    }
  }

  std::shared_ptr<T> getThresholdedImage() const {
    return SubtractImage<typename T::PixelType>::create(getFilteredImage(), getThresholdMap());
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

  typename WeightImage::PixelType getWeightThreshold() const {
    return m_weight_threshold;
  }

  typename T::PixelType getBackgroundRMS() const {
    return m_background_rms_map != nullptr ? m_background_rms_map->getValue(0,0) : 1;
  }

  std::shared_ptr<T> getBackgroundRMSMap() const {
    if (m_background_rms_map != nullptr) {
      return m_background_rms_map;
    } else {
      return ConstantImage<typename T::PixelType>::create(
          m_image->getWidth(), m_image->getHeight(), getBackgroundRMS());
    }
  }

  typename T::PixelType getBackgroundLevel() const {
    return m_background_level_map != nullptr ? m_background_level_map->getValue(0,0) : 0;
  }

  std::shared_ptr<T> getBackgroundLevelMap() const {
    if (m_background_level_map != nullptr) {
      return m_background_level_map;
    } else {
      return ConstantImage<typename T::PixelType>::create(
          m_image->getWidth(), m_image->getHeight(), getBackgroundLevel());
    }
  }

  std::shared_ptr<T> getThresholdMap() const {
    return MultiplyImage<typename T::PixelType>::create(getBackgroundRMSMap(), 1.5); // FIXME
  }

  void applyFilter(const ImageProcessing<typename T::PixelType>& image_processing) {
    m_filtered_image = image_processing.processImage(getSubtractedImage());
  }

  typename T::PixelType getDetectionThreshold() const {
    return m_detection_threshold;
  }

  void setDetectionThreshold(typename T::PixelType detection_threshold) {
    m_detection_threshold = detection_threshold;
  }

  void setBackgroundRMS(std::shared_ptr<T> background_rms_map =  nullptr) {
    m_background_rms_map = background_rms_map;
    m_detection_threshold = getBackgroundRMS() * 1.5; // FIXME temporary
  }

  void setBackgroundLevel(std::shared_ptr<T> background_level_map = nullptr) {
    m_background_level_map = background_level_map;
  }

  SeFloat getGain() const {
    return m_gain;
  }

  SeFloat getSaturation() const {
    return m_saturation;
  }

private:
  std::shared_ptr<T> m_image;
  std::shared_ptr<T> m_filtered_image;

  // background maps
  std::shared_ptr<T> m_background_level_map;
  std::shared_ptr<T> m_background_rms_map;

  std::shared_ptr<WeightImage> m_weight_image;
  std::shared_ptr<CoordinateSystem> m_coordinate_system;

//  typename T::PixelType m_background_level;
//  typename T::PixelType m_background_rms;

  typename T::PixelType m_detection_threshold;

  bool m_is_weight_absolute;
  typename WeightImage::PixelType m_weight_threshold;

  SeFloat m_gain;
  SeFloat m_saturation;
};

using DetectionImageFrame = Frame<DetectionImage>;
using MeasurementImageFrame = Frame<MeasurementImage>;

}

#endif /* _SEFRAMEWORK_FRAME_FRAME_H_ */
