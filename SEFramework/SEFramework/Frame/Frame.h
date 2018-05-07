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
#include "SEFramework/Image/InterpolatedImage.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

template<typename T>
class Frame {

public:
  class ImageFilter {
  public:
    virtual std::shared_ptr<Image<typename T::PixelType>> processImage(std::shared_ptr<Image<typename T::PixelType>> image, std::shared_ptr<Image<typename T::PixelType>> variance, typename T::PixelType threshold) const = 0;
  };

  Frame(std::shared_ptr<T> detection_image,
        std::shared_ptr<WeightImage> variance_map,
        WeightImage::PixelType variance_threshold,
        std::shared_ptr<CoordinateSystem> coordinate_system,
        SeFloat gain, SeFloat saturation, bool interpolate)
          : m_image(detection_image),
            m_variance_map(variance_map),
            m_coordinate_system(coordinate_system),
            m_gain(gain),
            m_saturation(saturation),
            m_detection_threshold(0),
            m_variance_threshold(variance_threshold),
            m_use_interpolation(interpolate)
            {}

  // FIXME: this simplified version is used in unit tests, get rid of it
  Frame(std::shared_ptr<T> detection_image,
        std::shared_ptr<CoordinateSystem> coordinate_system = nullptr,
        std::shared_ptr<WeightImage> variance_map = nullptr)
          : m_image(detection_image),
            m_variance_map(variance_map),
            m_coordinate_system(coordinate_system),
            m_gain(0),
            m_saturation(0),
            m_detection_threshold(0),
            m_variance_threshold(1e6),
            m_use_interpolation(false)
            {
              if (variance_map==nullptr) {
                m_variance_map = ConstantImage<WeightImage::PixelType>::create(detection_image->getWidth(), detection_image->getHeight(), .0001);
              }
            }

  // Just the original image
  std::shared_ptr<T> getOriginalImage() const {
    return m_image;
  }

  std::shared_ptr<T> getInterpolatedImage() const {
    if (m_interpolated_image == nullptr) {
      const_cast<Frame<T>*>(this)->m_interpolated_image = InterpolatedImage<typename T::PixelType>::create(
          m_image, m_variance_map, getVarianceThreshold());
    }

    return m_interpolated_image;
  }

  // Get the image with background subtraction
  std::shared_ptr<T> getSubtractedImage() const {
    if (m_use_interpolation) {
      return SubtractImage<typename T::PixelType>::create(getInterpolatedImage(), getBackgroundLevelMap());
    } else {
      return SubtractImage<typename T::PixelType>::create(getOriginalImage(), getBackgroundLevelMap());
    }
  }

//  std::shared_ptr<T> getSubtractedInterpolatedImage() const {
//    return SubtractImage<typename T::PixelType>::create(getInterpolatedImage(), getBackgroundLevelMap());
//  }

  // A filter is applied to the subtracted image
  std::shared_ptr<T> getFilteredImage() const {
    if (m_filtered_image == nullptr) {
      const_cast<Frame<T>*>(this)->applyFilter();
    }
    return m_filtered_image;
  }

  std::shared_ptr<T> getThresholdedImage() const {
    return ThresholdedImage<typename T::PixelType>::create(getFilteredImage(), getVarianceMap(), m_detection_threshold);
  }

  std::shared_ptr<CoordinateSystem> getCoordinateSystem() const {
    return m_coordinate_system;
  }

  std::shared_ptr<WeightImage> getVarianceMap() const {
    if (m_use_interpolation) {
      return InterpolatedImage<WeightImage::PixelType>::create(m_variance_map, m_variance_map, getVarianceThreshold());
    } else {
      return m_variance_map;
    }
  }

  std::shared_ptr<WeightImage> getUninterpolatedVarianceMap() const {
    return m_variance_map;
  }

  void setVarianceMap(std::shared_ptr<WeightImage> variance_map) {
    m_variance_map = variance_map;

    // resets the interpolated image cache and filtered image
    m_interpolated_image = nullptr;
    m_filtered_image = nullptr;
  }

  typename WeightImage::PixelType getVarianceThreshold() const {
    return m_variance_threshold;
  }

  void setVarianceThreshold(WeightImage::PixelType threshold) {
    m_variance_threshold = threshold;

    // resets the interpolated image cache and filtered image
    m_interpolated_image = nullptr;
    m_filtered_image = nullptr;
  }

  std::shared_ptr<T> getBackgroundLevelMap() const {
    if (m_background_level_map != nullptr) {
      return m_background_level_map;
    } else {
      // background level = 0 by default
      return ConstantImage<typename T::PixelType>::create(m_image->getWidth(), m_image->getHeight(), 0);
    }
  }

  typename T::PixelType getDetectionThreshold() const {
		return sqrt(m_variance_map->getValue(0,0)) * m_detection_threshold;
  }

  void setDetectionThreshold(typename T::PixelType detection_threshold) {
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

  void setFilter(std::shared_ptr<ImageFilter> filter) {
    m_filter = filter;
    m_filtered_image = nullptr;
  }

private:

  void applyFilter() {
    if (m_filter != nullptr) {
      m_filtered_image = m_filter->processImage(getSubtractedImage(), getVarianceMap(), getVarianceThreshold());
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
  bool m_use_interpolation;

  typename T::PixelType m_detection_threshold;
  typename WeightImage::PixelType m_variance_threshold;

  std::shared_ptr<ImageFilter> m_filter;
  std::shared_ptr<T> m_interpolated_image;
  std::shared_ptr<T> m_filtered_image;
};

using DetectionImageFrame = Frame<DetectionImage>;
using MeasurementImageFrame = Frame<MeasurementImage>;

}

#endif /* _SEFRAMEWORK_FRAME_FRAME_H_ */
