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
#include "SEFramework/Image/SnrImage.h"
#include "SEFramework/Image/InterpolatedImage.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

template<typename T>
class Frame {

public:
  class ImageFilter {
  public:
    virtual std::shared_ptr<Image<T>> processImage(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> variance, T threshold) const = 0;
  };

  Frame(std::shared_ptr<Image<T>> detection_image,
        std::shared_ptr<WeightImage> variance_map,
        WeightImage::PixelType variance_threshold,
        std::shared_ptr<CoordinateSystem> coordinate_system,
        SeFloat gain, SeFloat saturation, int interpolation_gap)
          : m_image(detection_image),
            m_variance_map(variance_map),
            m_coordinate_system(coordinate_system),
            m_gain(gain),
            m_saturation(saturation),
            m_detection_threshold(0),
            m_variance_threshold(variance_threshold),
            m_interpolation_gap(interpolation_gap)
            {}

  // FIXME: this simplified version is used in unit tests, get rid of it
  Frame(std::shared_ptr<Image<T>> detection_image,
        std::shared_ptr<CoordinateSystem> coordinate_system = nullptr,
        std::shared_ptr<WeightImage> variance_map = nullptr)
          : m_image(detection_image),
            m_variance_map(variance_map),
            m_coordinate_system(coordinate_system),
            m_gain(0),
            m_saturation(0),
            m_detection_threshold(0),
            m_variance_threshold(1e6),
            m_interpolation_gap(0)
            {
              if (variance_map==nullptr) {
                m_variance_map = ConstantImage<WeightImage::PixelType>::create(detection_image->getWidth(), detection_image->getHeight(), .0001);
              }
            }

  //
  // Methods to get the image in one form or another
  //

  // Just the original image
  std::shared_ptr<Image<T>> getOriginalImage() const {
    return m_image;
  }

  // Returns the image with bad pixels interpolated (if interpolation is active, otherwise returns original)
  std::shared_ptr<Image<T>> getInterpolatedImage() const {
    if (m_interpolation_gap > 0) {
      if (m_interpolated_image == nullptr) {
        const_cast<Frame<T>*>(this)->m_interpolated_image = InterpolatedImage<T>::create(
            getOriginalImage(), getUnfilteredVarianceMap(), getVarianceThreshold(), m_interpolation_gap);
      }
      return m_interpolated_image;
    } else {
      return getOriginalImage();
    }
  }

  // Get the image with the background subtracted
  std::shared_ptr<Image<T>> getSubtractedImage() const {
    return SubtractImage<T>::create(getInterpolatedImage(), getBackgroundLevelMap());
  }

  // Get the image with a filter applied to the subtracted image
  std::shared_ptr<Image<T>> getFilteredImage() const {
    if (m_filtered_image == nullptr) {
      const_cast<Frame<T>*>(this)->applyFilter();
    }
    return m_filtered_image;
  }

  // Get the filtered image with the detection threshold subtracted from it
  std::shared_ptr<Image<T>> getThresholdedImage() const {
    return ThresholdedImage<T>::create(getFilteredImage(), getVarianceMap(), m_detection_threshold);
  }

  // Get the SNR image
  std::shared_ptr<Image<T>> getSnrImage() const {
    return SnrImage<T>::create(getFilteredImage(), getVarianceMap());
  }

  //
  // Methods to get the image in one form or another
  //

  std::shared_ptr<WeightImage> getVarianceMap() const {
    if (m_filtered_variance_map == nullptr) {
      const_cast<Frame<T>*>(this)->applyFilter();
    }
    return m_filtered_variance_map;
  }

  std::shared_ptr<WeightImage> getUnfilteredVarianceMap() const {
    if (m_interpolation_gap > 0) {
      return InterpolatedImage<WeightImage::PixelType>::create(m_variance_map, m_variance_map, getVarianceThreshold(), m_interpolation_gap);
    } else {
      return m_variance_map;
    }
  }

  std::shared_ptr<WeightImage> getUninterpolatedVarianceMap() const {
    return m_variance_map;
  }

  //
  // Methods to get frame metadata
  //

  std::shared_ptr<CoordinateSystem> getCoordinateSystem() const {
    return m_coordinate_system;
  }

  typename WeightImage::PixelType getVarianceThreshold() const {
    return m_variance_threshold;
  }

  SeFloat getGain() const {
    return m_gain;
  }

  SeFloat getSaturation() const {
    return m_saturation;
  }

  T getDetectionThreshold() const {
    // FIXME using the 0,0 pixel makes no sense
    return sqrt(m_variance_map->getValue(0,0)) * m_detection_threshold;
  }

  //
  // Setters
  //

  void setVarianceMap(std::shared_ptr<WeightImage> variance_map) {
    m_variance_map = variance_map;

    // resets the interpolated image cache and filtered image
    m_interpolated_image = nullptr;
    m_filtered_image = nullptr;
    m_filtered_variance_map = nullptr;
  }

  void setVarianceThreshold(WeightImage::PixelType threshold) {
    m_variance_threshold = threshold;

    // resets the interpolated image cache and filtered image
    m_interpolated_image = nullptr;
    m_filtered_image = nullptr;
    m_filtered_variance_map = nullptr;
  }

  std::shared_ptr<Image<T>> getBackgroundLevelMap() const {
    if (m_background_level_map != nullptr) {
      return m_background_level_map;
    } else {
      // background level = 0 by default
      return ConstantImage<T>::create(m_image->getWidth(), m_image->getHeight(), 0);
    }
  }

  void setDetectionThreshold(T detection_threshold) {
    m_detection_threshold = detection_threshold;
  }

  void setBackgroundLevel(std::shared_ptr<Image<T>> background_level_map) {
    m_background_level_map = background_level_map;
    m_filtered_image = nullptr;
  }

  void setFilter(std::shared_ptr<ImageFilter> filter) {
    m_filter = filter;
    m_filtered_image = nullptr;
    m_filtered_variance_map = nullptr;
  }

private:

  void applyFilter() {
    if (m_filter != nullptr) {
      m_filtered_image = m_filter->processImage(getSubtractedImage(), getUnfilteredVarianceMap(), getVarianceThreshold());
      m_filtered_variance_map = m_filter->processImage(getUnfilteredVarianceMap(), getUnfilteredVarianceMap(), getVarianceThreshold());
    } else {
      m_filtered_image = getSubtractedImage();
      m_filtered_variance_map = getUnfilteredVarianceMap();
    }
  }

  std::shared_ptr<Image<T>> m_image;
  std::shared_ptr<WeightImage> m_variance_map;
  std::shared_ptr<Image<T>> m_background_level_map;

  std::shared_ptr<CoordinateSystem> m_coordinate_system;

  SeFloat m_gain;
  SeFloat m_saturation;

  T m_detection_threshold;
  typename WeightImage::PixelType m_variance_threshold;

  int m_interpolation_gap; // max interpolation gap, 0 == no interpolation

  std::shared_ptr<ImageFilter> m_filter;
  std::shared_ptr<Image<T>> m_interpolated_image;
  std::shared_ptr<Image<T>> m_filtered_image;
  std::shared_ptr<Image<T>> m_filtered_variance_map;
};

using DetectionImageFrame = Frame<DetectionImage::PixelType>;
using MeasurementImageFrame = Frame<MeasurementImage::PixelType>;

}

#endif /* _SEFRAMEWORK_FRAME_FRAME_H_ */
