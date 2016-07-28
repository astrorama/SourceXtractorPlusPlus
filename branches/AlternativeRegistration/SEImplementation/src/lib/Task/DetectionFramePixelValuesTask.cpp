/**
 * @file src/lib/Task/DetectionFramePixelValuesTask.cpp
 * @date 06/16/16
 * @author mschefer
 */
#include <memory>

#include "SEImplementation/Property/DetectionFramePixelValues.h"
#include "SEImplementation/Task/DetectionFramePixelValuesTask.h"

namespace SExtractor {

void DetectionFramePixelValuesTask::computeProperties(PixelSourceInterface& source) const {
  std::vector<double> values;

  for (auto pixel_coord : source.getPixels()) {
    values.push_back(m_image->getValue(pixel_coord.m_x, pixel_coord.m_y));
  }

  source.setProperty(std::unique_ptr<DetectionFramePixelValues>(new DetectionFramePixelValues(std::move(values))));
}

DetectionFramePixelValuesTask::DetectionFramePixelValuesTask(std::shared_ptr<DetectionImage> image) : m_image(image) {
}

} // SEImplementation namespace



