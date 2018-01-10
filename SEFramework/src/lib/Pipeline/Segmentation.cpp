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

void Segmentation::processFrame(std::shared_ptr<DetectionImageFrame> frame) const {
  if (m_filter_image_processing != nullptr) {
    frame->setFilter(m_filter_image_processing);
  }

  if (m_labelling != nullptr) {
    LabellingListener listener(*this, frame);
    m_labelling->labelImage(listener, frame);
  }
}

}
