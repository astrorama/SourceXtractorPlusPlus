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

#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Image/ImageAccessor.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/FunctionalImage.h"
#include "SEFramework/Image/InterpolatedImageSource.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/Image/ThresholdedImage.h"


namespace SourceXtractor {

template<typename T>
Frame<T>::Frame(std::shared_ptr<Image<T>> detection_image,
                std::shared_ptr<WeightImage> variance_map,
                WeightImage::PixelType variance_threshold,
                std::shared_ptr<CoordinateSystem> coordinate_system,
                SeFloat gain, SeFloat saturation, int interpolation_gap
):
  m_image(detection_image),
  m_variance_map(variance_map),
  m_coordinate_system(coordinate_system),
  m_gain(gain),
  m_saturation(saturation),
  m_background_rms(0),
  m_detection_threshold(0),
  m_variance_threshold(variance_threshold),
  m_interpolation_gap(interpolation_gap) {
  applyInterpolation();
  applyFilter();
}


template<typename T>
Frame<T>::Frame(std::shared_ptr<Image<T>> detection_image,
                std::shared_ptr<CoordinateSystem> coordinate_system,
                std::shared_ptr<WeightImage> variance_map
):
  m_image(detection_image),
  m_variance_map(variance_map),
  m_coordinate_system(coordinate_system),
  m_gain(0),
  m_saturation(0),
  m_background_rms(0),
  m_detection_threshold(0),
  m_variance_threshold(1e6),
  m_interpolation_gap(0) {
  if (variance_map == nullptr && detection_image != nullptr) {
    m_variance_map = ConstantImage<WeightImage::PixelType>::create(detection_image->getWidth(),
                                                                   detection_image->getHeight(), .0001);
  }
  applyInterpolation();
  applyFilter();
}

template<typename T>
std::shared_ptr<Image<T>> Frame<T>::getImage(FrameImageLayer layer) const {
  // FIXME replace switch with a better system
  switch(layer) {
  default:
  case LayerOriginalImage:
    return getOriginalImage();
    break;
  case LayerInterpolatedImage:
    return getInterpolatedImage();
    break;
  case LayerSubtractedImage:
    return getSubtractedImage();
    break;
  case LayerFilteredImage:
    return getFilteredImage();
    break;
  case LayerThresholdedImage:
    return getThresholdedImage();
    break;
  case LayerSignalToNoiseMap:
    return getSnrImage();
    break;
  case LayerOriginalVarianceMap:
    return getOriginalVarianceMap();
    break;
  case LayerUnfilteredVarianceMap:
    return getUnfilteredVarianceMap();
    break;
  case LayerVarianceMap:
    return getVarianceMap();
    break;
  case LayerDetectionThresholdMap:
    return getDetectionThresholdMap();
    break;
  }
}

template<typename T>
std::shared_ptr<Image<T>> Frame<T>::getInterpolatedImage() const {
  if (m_interpolated_image > 0) {
    return m_interpolated_image;
  }
  else {
    return m_image;
  }
}


template<typename T>
std::shared_ptr<Image<T>> Frame<T>::getSubtractedImage() const {
  return SubtractImage<T>::create(getInterpolatedImage(), getBackgroundLevelMap());
}


template<typename T>
std::shared_ptr<Image<T>> Frame<T>::getFilteredImage() const {
  return m_filtered_image;
}


template<typename T>
std::shared_ptr<Image<T>> Frame<T>::getThresholdedImage() const {
  return ThresholdedImage<T>::create(getFilteredImage(), getVarianceMap(), m_detection_threshold);
}


template<typename T>
std::shared_ptr<Image<T>> Frame<T>::getSnrImage() const {
  return SnrImage<T>::create(getFilteredImage(), getVarianceMap());
}


template<typename T>
std::shared_ptr<WeightImage> Frame<T>::getVarianceMap() const {
  return m_filtered_variance_map;
}


template<typename T>
std::shared_ptr<WeightImage> Frame<T>::getUnfilteredVarianceMap() const {
  if (m_interpolated_variance) {
    return m_interpolated_variance;
  }
  else {
    return m_variance_map;
  }
}


template<typename T>
std::shared_ptr<Image<T>> Frame<T>::getDetectionThresholdMap() const {
  struct ThresholdOperation {
    static T process(const T& a, const T& b) { return sqrt(a) * b; }
  };

  using ThresholdImage = ProcessedImage<T, ThresholdOperation>;
  return ThresholdImage::create(m_variance_map, m_detection_threshold);
}


template<typename T>
void Frame<T>::setVarianceMap(std::shared_ptr<WeightImage> variance_map) {
  m_variance_map = variance_map;

  // resets the interpolated image cache and filtered image
  m_interpolated_image = nullptr;
  m_interpolated_variance = nullptr;
  m_filtered_image = nullptr;
  m_filtered_variance_map = nullptr;

  applyInterpolation();
  applyFilter();
}


template<typename T>
void Frame<T>::setVarianceThreshold(WeightImage::PixelType threshold) {

  // set the variance threshold if it make sense
  if (m_variance_threshold < std::numeric_limits<WeightImage::PixelType>::max()){
    m_variance_threshold = threshold;
  }
  else{
    return;
  }

  // resets the interpolated image cache and filtered image
  m_interpolated_image = nullptr;
  m_interpolated_variance = nullptr;
  m_filtered_image = nullptr;
  m_filtered_variance_map = nullptr;

  applyInterpolation();
  applyFilter();
}


template<typename T>
std::shared_ptr<Image<T>> Frame<T>::getBackgroundLevelMap() const {
  if (m_background_level_map != nullptr) {
    return m_background_level_map;
  }
  else {
    // background level = 0 by default
    return ConstantImage<T>::create(m_image->getWidth(), m_image->getHeight(), 0);
  }
}


template<typename T>
void Frame<T>::setDetectionThreshold(T detection_threshold) {
  m_detection_threshold = detection_threshold;
}


template<typename T>
void Frame<T>::setBackgroundLevel(T background_level) {
  setBackgroundLevel(ConstantImage<T>::create(m_image->getWidth(), m_image->getHeight(), background_level), 0.);
}


template<typename T>
void Frame<T>::setBackgroundLevel(std::shared_ptr<Image<T>> background_level_map, T background_rms) {
  m_background_level_map = background_level_map;
  m_background_rms = background_rms;
  m_filtered_image = nullptr;
  m_filtered_variance_map = nullptr;

  applyFilter();
}


template<typename T>
void Frame<T>::setFilter(std::shared_ptr<ImageFilter> filter) {
  m_filter = filter;
  m_filtered_image = nullptr;
  m_filtered_variance_map = nullptr;
  applyFilter();
}


template<typename T>
void Frame<T>::setLabel(const std::string& label) {
  m_label = label;
}


template<typename T>
void Frame<T>::applyFilter() {
  if (m_filter != nullptr) {
    m_filtered_image = m_filter->processImage(getSubtractedImage(), getUnfilteredVarianceMap(),
                                              m_variance_threshold);
    auto filtered_variance_map = m_filter->processImage(getUnfilteredVarianceMap(),
                                                        getUnfilteredVarianceMap(),
                                                        m_variance_threshold);
    m_filtered_variance_map = FunctionalImage<T>::create(
      filtered_variance_map, [](int, int, WeightImage::PixelType v) {
        return std::max(v, 0.f);
      }
    );
  }
  else {
    m_filtered_image = getSubtractedImage();
    m_filtered_variance_map = getUnfilteredVarianceMap();
  }
}

template<typename T>
void Frame<T>::applyInterpolation() {
  if (!m_interpolated_variance) {
    m_interpolated_variance = BufferedImage<WeightImage::PixelType>::create(
      std::make_shared<InterpolatedImageSource<WeightImage::PixelType>>(
        m_variance_map, m_variance_map, m_variance_threshold, m_interpolation_gap)
    );
  }
  if (!m_interpolated_image) {
    m_interpolated_image = BufferedImage<T>::create(
      std::make_shared<InterpolatedImageSource<T>>(
        m_image, m_variance_map, m_variance_threshold, m_interpolation_gap)
    );
  }
}

template
class Frame<SeFloat>;

} // end namespace SourceXtractor
