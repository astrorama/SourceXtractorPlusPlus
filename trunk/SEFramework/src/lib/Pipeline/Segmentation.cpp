/**
 * @file src/lib/Pipeline/Segmentation.cpp
 * @date 09/07/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Segmentation.h"

namespace SExtractor {

Segmentation::Segmentation(std::shared_ptr<DetectionImageProcessing> image_processing)
    : m_filter_image_processing(image_processing) {
}

void Segmentation::processFrame(std::shared_ptr<DetectionImageFrame> frame) {

  m_detection_frame = frame; // FIXME not re-entrant!!

  if (m_filter_image_processing != nullptr) {
    frame->applyFilter(*m_filter_image_processing);
  }

  if (m_labelling != nullptr) {
    m_labelling->labelImage(frame);
  }
}

}
