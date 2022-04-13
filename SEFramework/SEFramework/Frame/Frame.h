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
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SourceXtractor {

enum FrameImageLayer {
  LayerOriginalImage = 0,
  LayerInterpolatedImage,
  LayerSubtractedImage,
  LayerFilteredImage,
  LayerThresholdedImage,
  LayerSignalToNoiseMap,
  LayerOriginalVarianceMap,
  LayerUnfilteredVarianceMap,
  LayerVarianceMap,
  LayerDetectionThresholdMap
};

template<typename T>
class Frame {

public:

  class ImageFilter {
  public:
    virtual ~ImageFilter() = default;
    virtual std::shared_ptr<Image<T>> processImage(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> variance, T threshold) const = 0;
  };

  Frame(std::shared_ptr<Image<T>> detection_image,
        std::shared_ptr<WeightImage> variance_map,
        WeightImage::PixelType variance_threshold,
        std::shared_ptr<CoordinateSystem> coordinate_system,
        SeFloat gain, SeFloat saturation, int interpolation_gap);

  // FIXME: this simplified version is used in unit tests, get rid of it
  explicit Frame(std::shared_ptr<Image<T>> detection_image,
                 std::shared_ptr<CoordinateSystem> coordinate_system = nullptr,
                 std::shared_ptr<WeightImage> variance_map = nullptr);

  //
  // Methods to get the image in one form or another
  //

  std::shared_ptr<Image<T>> getImage(FrameImageLayer layer) const;


  // Just the original image
  std::shared_ptr<Image<T>> getOriginalImage() const {
    return m_image;
  }

  size_t getHduIndex() const {
   return m_hdu_index;
  }

  void setHduIndex(size_t hdu_index) {
    m_hdu_index = hdu_index;
  }

  // Metadata of the original image
  const std::map<std::string, MetadataEntry>& getMetadata() const {
    return m_metadata;
  };

  void setMetadata(const std::map<std::string, MetadataEntry>& metadata) {
    m_metadata = metadata;
  };

  // Returns the image with bad pixels interpolated (if interpolation is active, otherwise returns original)
  std::shared_ptr<Image<T>> getInterpolatedImage() const;

  // Get the image with the background subtracted
  std::shared_ptr<Image<T>> getSubtractedImage() const;

  // Get the image with a filter applied to the subtracted image
  std::shared_ptr<Image<T>> getFilteredImage() const;

  // Get the filtered image with the detection threshold subtracted from it
  std::shared_ptr<Image<T>> getThresholdedImage() const;

  // Get the SNR image
  std::shared_ptr<Image<T>> getSnrImage() const;

  //
  // Methods to get the image in one form or another
  //

  std::shared_ptr<WeightImage> getVarianceMap() const;

  std::shared_ptr<WeightImage> getUnfilteredVarianceMap() const;

  std::shared_ptr<WeightImage> getOriginalVarianceMap() const {
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

  SeFloat getBackgroundMedianRms() const {
    return m_background_rms;
  }

  std::shared_ptr<Image<T>> getDetectionThresholdMap() const;

  std::string getLabel() const {
    return m_label;
  }

  //
  // Setters
  //

  void setVarianceMap(std::shared_ptr<WeightImage> variance_map);

  void setVarianceThreshold(WeightImage::PixelType threshold);

  std::shared_ptr<Image<T>> getBackgroundLevelMap() const;

  void setDetectionThreshold(T detection_threshold);

  void setBackgroundLevel(T background_level);

  void setBackgroundLevel(std::shared_ptr<Image<T>> background_level_map, T background_rms);

  void setFilter(std::shared_ptr<ImageFilter> filter);

  void setLabel(const std::string &label);

private:

  void applyFilter();
  void applyInterpolation();

  std::shared_ptr<Image<T>> m_image;
  std::shared_ptr<WeightImage> m_variance_map;
  std::shared_ptr<Image<T>> m_background_level_map;

  std::shared_ptr<CoordinateSystem> m_coordinate_system;

  SeFloat m_gain;
  SeFloat m_saturation;
  SeFloat m_background_rms;

  T m_detection_threshold;
  typename WeightImage::PixelType m_variance_threshold;

  int m_interpolation_gap; // max interpolation gap, 0 == no interpolation

  std::shared_ptr<ImageFilter> m_filter;
  std::shared_ptr<Image<T>> m_interpolated_image;
  std::shared_ptr<Image<WeightImage::PixelType>> m_interpolated_variance;
  std::shared_ptr<Image<T>> m_filtered_image;
  std::shared_ptr<Image<T>> m_filtered_variance_map;

  std::string m_label;
  size_t m_hdu_index = 0;
  std::map<std::string, MetadataEntry> m_metadata {};
};

using DetectionImageFrame = Frame<DetectionImage::PixelType>;
using MeasurementImageFrame = Frame<MeasurementImage::PixelType>;

}

#endif /* _SEFRAMEWORK_FRAME_FRAME_H_ */
