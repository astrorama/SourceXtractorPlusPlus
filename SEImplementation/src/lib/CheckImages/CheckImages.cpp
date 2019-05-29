/*
 * CheckImages.cpp
 *
 *  Created on: May 30, 2017
 *      Author: mschefer
 */

#include "SEFramework/Image/FitsWriter.h"
#include "SEFramework/Image/SubtractImage.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/CheckImagesConfig.h"

#include "SEImplementation/CheckImages/CheckImages.h"

namespace SExtractor {

std::unique_ptr<CheckImages> CheckImages::m_instance;

CheckImages::CheckImages() {
}

void CheckImages::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<CheckImagesConfig>();
}

std::shared_ptr<WriteableImage<SeFloat>> CheckImages::getWriteableCheckImage(std::string id, int width, int height) {
  if (m_custom_images.count(id) != 0) {
    auto image = std::dynamic_pointer_cast<WriteableImage<SeFloat>>(std::get<0>(m_custom_images[id]));
    if (image != nullptr) {
      return image;
    }
  }


  auto image = FitsWriter::newImage<SeFloat>(id + ".fits",
      width, height);
  m_custom_images[id] = std::make_tuple(image, false);

  return image;
}

void CheckImages::setCustomCheckImage(std::string id, std::shared_ptr<Image<SeFloat>> image) {
  m_custom_images[id] = std::make_tuple(image, true);
}

void CheckImages::configure(Euclid::Configuration::ConfigManager& manager) {
  m_detection_image = manager.getConfiguration<DetectionImageConfig>().getDetectionImage();
  auto& config = manager.getConfiguration<CheckImagesConfig>();

  m_model_fitting_image_filename = config.getModelFittingImageFilename();
  m_residual_filename = config.getModelFittingResidualFilename();
  m_model_background_filename = config.getModelBackgroundFilename();
  m_model_variance_filename = config.getModelVarianceFilename();
  m_segmentation_filename = config.getSegmentationFilename();
  m_partition_filename = config.getPartitionFilename();
  m_group_filename = config.getGroupFilename();
  m_filtered_filename = config.getFilteredFilename();
  m_thresholded_filename = config.getThresholdedFilename();
  m_auto_aperture_filename = config.getAutoApertureFilename();
  m_aperture_filename = config.getApertureFilename();
  m_moffat_filename = config.getMoffatFilename();

  m_coordinate_system = manager.getConfiguration<DetectionImageConfig>().getCoordinateSystem();

  if (m_model_fitting_image_filename != "") {
    m_check_image_model_fitting = FitsWriter::newImage<DetectionImage::PixelType>(m_model_fitting_image_filename.native(),
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system);
  }
  else if (m_residual_filename != "") {
    m_check_image_model_fitting = FitsWriter::newTemporaryImage<DetectionImage::PixelType>(
      "sextractor_check_model_%%%%%%.fits", m_detection_image->getWidth(), m_detection_image->getHeight());
  }

  if (m_segmentation_filename != "") {
    m_segmentation_image = FitsWriter::newImage<unsigned int>(m_segmentation_filename.native(),
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system);
  }

  if (m_partition_filename != "") {
    m_partition_image = FitsWriter::newImage<unsigned int>(m_partition_filename.native(),
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system);
  }

  if (m_group_filename != "") {
    m_group_image = FitsWriter::newImage<unsigned int>(m_group_filename.native(),
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system);
  }

  if (m_auto_aperture_filename != "") {
    m_auto_aperture_image = FitsWriter::newImage<unsigned int>(m_auto_aperture_filename.native(),
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system);
  }

  if (m_aperture_filename != "") {
    m_aperture_image = FitsWriter::newImage<unsigned int>(m_aperture_filename.native(),
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system
    );
  }

  if (m_moffat_filename != "") {
    m_moffat_image = FitsWriter::newImage<SeFloat>(m_moffat_filename.native(),
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system
    );
  }
}

std::shared_ptr<WriteableImage<unsigned int>>
CheckImages::getAutoApertureImage(std::shared_ptr<const MeasurementImageFrame> frame) {
  if (m_auto_aperture_filename.empty()) {
    return nullptr;
  }

  std::lock_guard<std::mutex> lock{m_access_mutex};

  auto i = m_measurement_auto_aperture_images.find(frame);
  if (i == m_measurement_auto_aperture_images.end()) {
    auto filename = m_auto_aperture_filename.stem();
    filename += "_" + frame->getLabel();
    filename.replace_extension(m_auto_aperture_filename.extension());
    auto frame_filename = m_auto_aperture_filename.parent_path() / filename;
    i = m_measurement_auto_aperture_images.emplace(
      std::make_pair(
        frame,
        FitsWriter::newImage<unsigned int>(
          frame_filename.native(),
          frame->getOriginalImage()->getWidth(),
          frame->getOriginalImage()->getHeight(),
          frame->getCoordinateSystem()
        ))).first;
  }
  return i->second;
}

std::shared_ptr<WriteableImage<unsigned int>>
CheckImages::getApertureImage(std::shared_ptr<const MeasurementImageFrame> frame) {
  if (m_aperture_filename.empty()) {
    return nullptr;
  }

  std::lock_guard<std::mutex> lock{m_access_mutex};

  auto i = m_measurement_aperture_images.find(frame);
  if (i == m_measurement_aperture_images.end()) {
    auto filename = m_aperture_filename.stem();
    filename += "_" + frame->getLabel();
    filename.replace_extension(m_aperture_filename.extension());
    auto frame_filename = m_aperture_filename.parent_path() / filename;
    i = m_measurement_aperture_images.emplace(
      std::make_pair(
        frame,
        FitsWriter::newImage<unsigned int>(
          frame_filename.native(),
          frame->getOriginalImage()->getWidth(),
          frame->getOriginalImage()->getHeight(),
          frame->getCoordinateSystem()
        ))).first;
  }
  return i->second;
}

void CheckImages::saveImages() {
  lock();

  // if possible, save the background image
  if (m_background_image != nullptr && m_model_background_filename != "") {
    FitsWriter::writeFile(*m_background_image, m_model_background_filename.native(), m_coordinate_system);
  }

  // if possible, save the variance image
  if (m_variance_image != nullptr && m_model_variance_filename != "") {
    FitsWriter::writeFile(*m_variance_image, m_model_variance_filename.native(), m_coordinate_system);
  }

  // if possible, save the filtered image
  if (m_filtered_image != nullptr && m_filtered_filename != "") {
    FitsWriter::writeFile(*m_filtered_image, m_filtered_filename.native(), m_coordinate_system);
  }

  // if possible, save the thresholded image
  if (m_thresholded_image != nullptr && m_thresholded_filename != "") {
    FitsWriter::writeFile(*m_thresholded_image, m_thresholded_filename.native(), m_coordinate_system);
  }

  // if possible, create and save the residual image
  if (m_check_image_model_fitting != nullptr && m_residual_filename != "") {
    auto residual_image = SubtractImage<SeFloat>::create(m_detection_image, m_check_image_model_fitting);
    FitsWriter::writeFile(*residual_image, m_residual_filename.native(), m_coordinate_system);
  }

  for (auto const& entry : m_custom_images) {
    if (std::get<1>(entry.second)) {
      auto filename = entry.first;
      if (!filename.has_extension())
        filename.replace_extension(".fits");
      FitsWriter::writeFile(*std::get<0>(entry.second), filename.native());
    }
  }

  unlock();
}

}
