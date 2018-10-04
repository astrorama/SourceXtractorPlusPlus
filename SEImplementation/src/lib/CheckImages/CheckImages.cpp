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

void CheckImages::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<CheckImagesConfig>();
}

void CheckImages::configure(Euclid::Configuration::ConfigManager& manager) {
  m_detection_image = manager.getConfiguration<DetectionImageConfig>().getDetectionImage();
  auto& config = manager.getConfiguration<CheckImagesConfig>();

  m_model_fitting_image_filename = config.getModelFittingImageFilename();
  m_residual_filename = config.getModelFittingResidualFilename();
  m_model_background_filename = config.getModelBackgroundFilename();
  m_model_variance_filename =  config.getModelVarianceFilename();
  m_segmentation_filename =  config.getSegmentationFilename();
  m_partition_filename =  config.getPartitionFilename();
  m_filtered_filename =  config.getFilteredFilename();

  m_coordinate_system = manager.getConfiguration<DetectionImageConfig>().getCoordinateSystem();

  if (m_model_fitting_image_filename != "") {
    m_check_image_model_fitting = FitsWriter::newImage<DetectionImage::PixelType>(m_model_fitting_image_filename,
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system);
  }
  else if (m_residual_filename != "") {
    m_check_image_model_fitting = FitsWriter::newTemporaryImage<DetectionImage::PixelType>(
        "sextractor_check_model_%%%%%%.fits", m_detection_image->getWidth(), m_detection_image->getHeight());
  }

  if (m_segmentation_filename != "") {
    m_segmentation_image = FitsWriter::newImage<unsigned int>(m_segmentation_filename,
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system);
  }

  if (m_partition_filename != "") {
    m_partition_image = FitsWriter::newImage<unsigned int>(m_partition_filename,
        m_detection_image->getWidth(), m_detection_image->getHeight(), m_coordinate_system);
  }
}

void CheckImages::saveImages() {
  lock();

  // if possible, save the background image
  if (m_background_image != nullptr && m_model_background_filename != "") {
    FitsWriter::writeFile(*m_background_image, m_model_background_filename, m_coordinate_system);
  }

  // if possible, save the variance image
  if (m_variance_image != nullptr && m_model_variance_filename != "") {
    FitsWriter::writeFile(*m_variance_image, m_model_variance_filename, m_coordinate_system);
  }

  // if possible, save the filtered image
  if (m_filtered_image != nullptr && m_filtered_filename != "") {
    FitsWriter::writeFile(*m_filtered_image, m_filtered_filename, m_coordinate_system);
  }

  // if possible, create and save the residual image
  if (m_check_image_model_fitting != nullptr && m_residual_filename != "") {
    auto residual_image = SubtractImage<SeFloat>::create(m_detection_image, m_check_image_model_fitting);
    FitsWriter::writeFile(*residual_image, m_residual_filename, m_coordinate_system);
  }

  unlock();
}

}
