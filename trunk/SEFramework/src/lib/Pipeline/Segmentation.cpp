/**
 * @file src/lib/Pipeline/Segmentation.cpp
 * @date 09/07/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Segmentation.h"

namespace SExtractor {

Segmentation::Segmentation(std::shared_ptr<DetectionImageProcessing> detection_image_processing,
    std::shared_ptr<DetectionImageProcessing> labelling_image_processing,
    std::shared_ptr<DetectionImageProcessing> thresholding_image_processing)
    : m_detection_image_processing(detection_image_processing),
      m_labelling_image_processing(labelling_image_processing),
      m_thresholding_image_processing(thresholding_image_processing) {
}

void Segmentation::processImage(std::shared_ptr<DetectionImage> image,
    std::shared_ptr<CoordinateSystem> coordinate_system) {

  m_detection_image_coordinate_system = coordinate_system;

  if (m_detection_image_processing != nullptr) {
    m_processed_detection_image = m_detection_image_processing->processImage(image);
  } else {
    m_processed_detection_image = image;
  }

  m_labelling_image = m_processed_detection_image;
  if (m_labelling != nullptr) {
    if (m_labelling_image_processing != nullptr) {
      m_labelling_image = m_labelling_image_processing->processImage(m_processed_detection_image);
    }

    auto thresholded_image = m_thresholding_image_processing->processImage(m_labelling_image);
    m_labelling->labelImage(*thresholded_image);
  }
}

}
