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
#include "SEFramework/Image/ThresholdedImage.h"
#include "SEFramework/Image/ImageProcessing.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

template<typename T>
class Frame {

public:

  Frame(std::shared_ptr<T> detection_image,
        std::shared_ptr<WeightImage> variance_map,
        bool is_weight_absolute,
        WeightImage::PixelType variance_threshold,
        std::shared_ptr<CoordinateSystem> coordinate_system,
        SeFloat gain, SeFloat saturation)
          : m_image(detection_image),
            m_variance_map(variance_map),
            m_coordinate_system(coordinate_system),
            m_gain(gain),
            m_saturation(saturation),
            m_variance_threshold(variance_threshold)
            {}

  Frame(std::shared_ptr<T> detection_image,
        std::shared_ptr<CoordinateSystem> coordinate_system = nullptr)
          : m_image(detection_image),
            m_variance_map(nullptr),
            m_coordinate_system(coordinate_system),
            m_gain(0),
            m_saturation(0),
            m_variance_threshold(0)
            {}

  // Just the original image
  std::shared_ptr<T> getOriginalImage() const {
    return m_image;
  }

  // Get the image with background subtraction
  std::shared_ptr<T> getSubtractedImage() const {
    return SubtractImage<typename T::PixelType>::create(getOriginalImage(), getBackgroundLevelMap());
  }

  // A filter is applied to the subtracted image
  std::shared_ptr<T> getFilteredImage() const {
    if (m_filtered_image == nullptr) {
      const_cast<Frame<T>*>(this)->applyFilter();
    }
    return m_filtered_image;
  }

  std::shared_ptr<T> getThresholdedImage() const {
    return SubtractImage<typename T::PixelType>::create(getFilteredImage(), 35);
    //return ThresholdedImage<typename T::PixelType>::create(getFilteredImage(), getVarianceMap(), 1.5);
  }

  std::shared_ptr<CoordinateSystem> getCoordinateSystem() const {
    return m_coordinate_system;
  }

  std::shared_ptr<WeightImage> getVarianceMap() const {
    return m_variance_map;
  }

  void setVarianceMap(std::shared_ptr<WeightImage> variance_map) {
    m_variance_map = variance_map;
  }

  typename WeightImage::PixelType getVarianceThreshold() const {
    return m_variance_threshold;
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

//  std::shared_ptr<T> getThresholdMap() const {
//    // FIXME !!!!!!!!!! BROKEN;
//    //return MultiplyImage<typename T::PixelType>::create(getBackgroundRMSMap(), 1.5); // FIXME
//    return ConstantImage<typename T::PixelType>::create(
//        m_image->getWidth(), m_image->getHeight(), 1.5 * sqrt(m_variance_map->getValue(0,0))); //FIXME tmp
//  }

  typename T::PixelType getDetectionThreshold() const {
    return sqrt(m_variance_map->getValue(0,0)) * 1.5;
    //return m_detection_threshold; // FIXME!!!!!!
  }

  void setDetectionThreshold(typename T::PixelType detection_threshold) {
    // FIXME this does nothing currently
    m_detection_threshold = detection_threshold;
  }

  void setBackgroundLevel(std::shared_ptr<T> background_level_map) {
    m_background_level_map = background_level_map;
    m_filtered_image = nullptr;
  }

  SeFloat getGain() const {
    return m_gain;
  }

  SeFloat getSaturation() const {
    return m_saturation;
  }

  void setFilter(std::shared_ptr<ImageProcessing<typename T::PixelType>> filter) {
    m_filter = filter;
    m_filtered_image = nullptr;
  }

private:

  void applyFilter() {
    if (m_filter != nullptr) {
      m_filtered_image = m_filter->processImage(getSubtractedImage());
    } else {
      m_filtered_image = getSubtractedImage();
    }
  }

  std::shared_ptr<T> m_image;
  std::shared_ptr<WeightImage> m_variance_map;
  std::shared_ptr<T> m_background_level_map;

  std::shared_ptr<CoordinateSystem> m_coordinate_system;

  SeFloat m_gain;
  SeFloat m_saturation;

  typename T::PixelType m_detection_threshold;
  typename WeightImage::PixelType m_variance_threshold;

  std::shared_ptr<ImageProcessing<typename T::PixelType>> m_filter;
  std::shared_ptr<T> m_filtered_image;
};

using DetectionImageFrame = Frame<DetectionImage>;
using MeasurementImageFrame = Frame<MeasurementImage>;

}

#endif /* _SEFRAMEWORK_FRAME_FRAME_H_ */
