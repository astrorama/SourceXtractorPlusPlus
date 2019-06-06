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
#include "SEFramework/Image/SubtractImage.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Frame/Frame.h"


namespace SExtractor {

class CheckImages : public Configurable {

public:

  virtual ~CheckImages() = default;

  void saveImages();

  std::shared_ptr<WriteableImage<unsigned int>> getSegmentationImage() const {
    return m_segmentation_image;
  }

  std::shared_ptr<WriteableImage<unsigned int>> getPartitionImage() const {
    return m_partition_image;
  }

  std::shared_ptr<WriteableImage<unsigned int>> getGroupImage() const {
    return m_group_image;
  }

  std::shared_ptr<WriteableImage<unsigned int>> getAutoApertureImage() const {
    return m_auto_aperture_image;
  }

  std::shared_ptr<WriteableImage<unsigned int>> getApertureImage() const {
    return m_aperture_image;
  }

  std::shared_ptr<WriteableImage<SeFloat>> getMoffatImage() const {
    return m_moffat_image;
  }

  std::shared_ptr<WriteableImage<unsigned int>>
  getAutoApertureImage(std::shared_ptr<const MeasurementImageFrame> frame);

  std::shared_ptr<WriteableImage<unsigned int>>
  getApertureImage(std::shared_ptr<const MeasurementImageFrame> frame);

  std::shared_ptr<WriteableImage<MeasurementImage::PixelType>>
  getModelFittingImage(std::shared_ptr<const MeasurementImageFrame> frame);

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

  void lock() {
    m_access_mutex.lock();
  }

  void unlock() {
    m_access_mutex.unlock();
  }

private:
  CheckImages();

  static std::unique_ptr<CheckImages> m_instance;

  // check image
  std::shared_ptr<WriteableImage<unsigned int>> m_segmentation_image;
  std::shared_ptr<WriteableImage<unsigned int>> m_partition_image;
  std::shared_ptr<WriteableImage<unsigned int>> m_group_image;
  std::shared_ptr<WriteableImage<unsigned int>> m_auto_aperture_image;
  std::shared_ptr<WriteableImage<unsigned int>> m_aperture_image;
  std::shared_ptr<WriteableImage<SeFloat>> m_moffat_image;
  std::map<std::shared_ptr<const MeasurementImageFrame>, decltype(m_aperture_image)> m_measurement_aperture_images;
  std::map<std::shared_ptr<const MeasurementImageFrame>, decltype(m_auto_aperture_image)> m_measurement_auto_aperture_images;
  std::map<std::shared_ptr<const MeasurementImageFrame>, std::shared_ptr<WriteableImage<MeasurementImage::PixelType>>> m_check_image_model_fitting;

  std::shared_ptr<DetectionImage> m_detection_image;
  std::shared_ptr<Image<SeFloat>> m_background_image;
  std::shared_ptr<Image<SeFloat>> m_filtered_image;
  std::shared_ptr<Image<SeFloat>> m_thresholded_image;
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
  boost::filesystem::path m_auto_aperture_filename;
  boost::filesystem::path m_aperture_filename;
  boost::filesystem::path m_moffat_filename;

  std::map<boost::filesystem::path, std::tuple<std::shared_ptr<Image<SeFloat>>, bool>> m_custom_images;

  std::mutex m_access_mutex;
};

}



#endif /* _SEIMPLEMENTATION_CHECKIMAGES_CHECKIMAGES_H_ */
