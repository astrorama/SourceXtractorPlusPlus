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
 * CheckImages.h
 *
 *  Created on: May 30, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CHECKIMAGES_CHECKIMAGES_H_
#define _SEIMPLEMENTATION_CHECKIMAGES_CHECKIMAGES_H_

#include <mutex>
#include <map>
#include <iostream>

#include <boost/filesystem/path.hpp>

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Frame/Frame.h"

#include "SEImplementation/Image/LockedWriteableImage.h"


namespace SourceXtractor {

class CheckImages : public Configurable {

public:

  virtual ~CheckImages() = default;

  void saveImages();

  std::shared_ptr<WriteableImage<unsigned int>> getSegmentationImage() const {
    if (m_segmentation_image != nullptr) {
      return LockedWriteableImage<unsigned int>::create(m_segmentation_image);
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<WriteableImage<unsigned int>> getPartitionImage() const {
    if (m_partition_image != nullptr) {
      return LockedWriteableImage<unsigned int>::create(m_partition_image);
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<WriteableImage<unsigned int>> getGroupImage() const {
    if (m_group_image != nullptr) {
      return LockedWriteableImage<unsigned int>::create(m_group_image);
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<WriteableImage<unsigned int>> getAutoApertureImage() const {
    if (m_auto_aperture_image != nullptr) {
      return LockedWriteableImage<unsigned int>::create(m_auto_aperture_image);
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<WriteableImage<unsigned int>> getApertureImage() const {
    if (m_aperture_image != nullptr) {
      return LockedWriteableImage<unsigned int>::create(m_aperture_image);
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<WriteableImage<SeFloat>> getMoffatImage() const {
    if (m_moffat_image != nullptr) {
      return LockedWriteableImage<SeFloat>::create(m_moffat_image);
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<WriteableImage<unsigned int>> getAutoApertureImage(unsigned int frame_number);

  std::shared_ptr<WriteableImage<unsigned int>> getApertureImage(unsigned int frame_number);

  std::shared_ptr<WriteableImage<MeasurementImage::PixelType>> getModelFittingImage(unsigned int frame_number);

  std::shared_ptr<WriteableImage<MeasurementImage::PixelType>> getPsfImage(unsigned int frame_number);

  void setBackgroundCheckImage(std::shared_ptr<Image<SeFloat>> background_image) {
    m_background_image = background_image;
  }

  void setVarianceCheckImage(std::shared_ptr<Image<SeFloat>> variance_image) {
    m_variance_image = variance_image;
  }

  void setFilteredCheckImage(std::shared_ptr<Image<SeFloat>> filtered_image) {
    m_filtered_image = filtered_image;
  }

  void setThresholdedCheckImage(std::shared_ptr<Image<SeFloat>> thresholded_image) {
    m_thresholded_image = thresholded_image;
  }

  void setSnrCheckImage(std::shared_ptr<Image<SeFloat>> snr_image) {
    m_snr_image = snr_image;
  }

  std::shared_ptr<WriteableImage<SeFloat>> getWriteableCheckImage(std::string id, int width, int height);
  void setCustomCheckImage(std::string id, std::shared_ptr<Image<SeFloat>> image);

  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  virtual void configure(Euclid::Configuration::ConfigManager& manager) override;

  static CheckImages& getInstance() {
    if (m_instance == nullptr) {
      m_instance.reset(new CheckImages);
    }

    return *m_instance;
  }

private:
  CheckImages();

  static std::unique_ptr<CheckImages> m_instance;

  struct FrameInfo {
    std::string m_label;
    int m_width, m_height;
    std::shared_ptr<CoordinateSystem> m_coordinate_system;
  };

  // check image
  std::shared_ptr<WriteableImage<unsigned int>> m_segmentation_image;
  std::shared_ptr<WriteableImage<unsigned int>> m_partition_image;
  std::shared_ptr<WriteableImage<unsigned int>> m_group_image;
  std::shared_ptr<WriteableImage<unsigned int>> m_auto_aperture_image;
  std::shared_ptr<WriteableImage<unsigned int>> m_aperture_image;
  std::shared_ptr<WriteableImage<SeFloat>> m_moffat_image;
  std::map<unsigned int, decltype(m_aperture_image)> m_measurement_aperture_images;
  std::map<unsigned int, decltype(m_auto_aperture_image)> m_measurement_auto_aperture_images;
  std::map<unsigned int, std::shared_ptr<WriteableImage<MeasurementImage::PixelType>>> m_check_image_model_fitting, m_check_image_psf;

  std::shared_ptr<DetectionImage> m_detection_image;
  std::shared_ptr<Image<SeFloat>> m_background_image;
  std::shared_ptr<Image<SeFloat>> m_filtered_image;
  std::shared_ptr<Image<SeFloat>> m_thresholded_image;
  std::shared_ptr<Image<SeFloat>> m_snr_image;
  std::shared_ptr<WeightImage> m_variance_image;
  std::shared_ptr<CoordinateSystem> m_coordinate_system;

  boost::filesystem::path m_model_fitting_image_filename;
  boost::filesystem::path m_residual_filename;
  boost::filesystem::path m_model_background_filename;
  boost::filesystem::path m_model_variance_filename;
  boost::filesystem::path m_segmentation_filename;
  boost::filesystem::path m_partition_filename;
  boost::filesystem::path m_group_filename;
  boost::filesystem::path m_filtered_filename;
  boost::filesystem::path m_thresholded_filename;
  boost::filesystem::path m_snr_filename;
  boost::filesystem::path m_auto_aperture_filename;
  boost::filesystem::path m_aperture_filename;
  boost::filesystem::path m_moffat_filename;
  boost::filesystem::path m_psf_filename;

  std::map<boost::filesystem::path, std::tuple<std::shared_ptr<Image<SeFloat>>, bool>> m_custom_images;

  std::vector<FrameInfo> m_measurement_frames;

  std::mutex m_access_mutex;
};

}



#endif /* _SEIMPLEMENTATION_CHECKIMAGES_CHECKIMAGES_H_ */
