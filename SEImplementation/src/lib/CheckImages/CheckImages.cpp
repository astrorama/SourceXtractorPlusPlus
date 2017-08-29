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

  if (m_model_fitting_image_filename != "" || m_residual_filename != "") {
    m_check_image_model_fitting = std::make_shared<VectorImage<DetectionImage::PixelType>>(
        m_detection_image->getWidth(), m_detection_image->getHeight());
  }
}

void CheckImages::saveImages() {
  if (m_check_image_model_fitting != nullptr && m_model_fitting_image_filename != "") {
    FitsWriter::writeFile(*m_check_image_model_fitting, m_model_fitting_image_filename);
  }

  if (m_check_image_model_fitting != nullptr && m_residual_filename != "") {
    SubtractImage<SeFloat> residual_image(m_detection_image, m_check_image_model_fitting);
    FitsWriter::writeFile(residual_image, m_residual_filename);
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
