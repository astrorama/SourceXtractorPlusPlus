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
  m_model_background_filename = config.getModelBackgroundFilename();
  m_model_variance_filename =  config.getModelVarianceFilename();

  if (m_model_fitting_image_filename != "" || m_residual_filename != "") {
    m_check_image_model_fitting = VectorImage<DetectionImage::PixelType>::create(
        m_detection_image->getWidth(), m_detection_image->getHeight());
  }
}

void CheckImages::saveImages() {
  // if possible, save the model image
  if (m_check_image_model_fitting != nullptr && m_model_fitting_image_filename != "") {
    FitsWriter::writeFile(*m_check_image_model_fitting, m_model_fitting_image_filename);
  }

  // if possible, save the background image
  if (m_background_image != nullptr && m_model_background_filename != "") {
    FitsWriter::writeFile(*m_background_image, m_model_background_filename);
  }

  // if possible, save the variance image
  if (m_variance_image != nullptr && m_model_variance_filename != "") {
    FitsWriter::writeFile(*m_variance_image, m_model_variance_filename);
  }

  // if possible, create and save the residual image
  if (m_check_image_model_fitting != nullptr && m_residual_filename != "") {
    auto residual_image = SubtractImage<SeFloat>::create(m_detection_image, m_check_image_model_fitting);
    FitsWriter::writeFile(*residual_image, m_residual_filename);
  }
}

/*

//  // saturate check image
//  for (int y=0; y < m_check_image->getHeight(); y++) {
//    for (int x=0; x < m_check_image->getWidth(); x++) {
//      int sat = 65535;
//      if (m_check_image->getValue(x,y) > sat) {
//        m_check_image->setValue(x,y,sat);
//      }
//    }
//  }


//  auto filtered_image = std::make_shared<VectorImage<DetectionImage::PixelType>> (*(group.cbegin()->getProperty<DetectionFrame>().getFrame()->getSubtractedImage()));
//  for (int y=0; y < filtered_image->getHeight(); y++) {
//    for (int x=0; x < filtered_image->getWidth(); x++) {
//      int sat = 65535;
//      if (filtered_image->getValue(x,y) < 0) {
//        filtered_image->setValue(x,y,0);
//      }
//      if (filtered_image->getValue(x,y) > sat) {
//        filtered_image->setValue(x,y,sat);
//      }
//    }
//  }
//  FitsWriter::writeFile(*filtered_image, "mf_checkimage.sub.fits");


 */

}
