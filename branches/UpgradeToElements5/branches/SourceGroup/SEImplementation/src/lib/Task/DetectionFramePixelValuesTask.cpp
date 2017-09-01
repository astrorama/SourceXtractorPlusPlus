/**
 * @file src/lib/Task/DetectionFramePixelValuesTask.cpp
 * @date 06/16/16
 * @author mschefer
 */
#include <memory>

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/DetectionFramePixelValues.h"
#include "SEImplementation/Task/DetectionFramePixelValuesTask.h"

namespace SExtractor {

void DetectionFramePixelValuesTask::computeProperties(SourceInterface& source) const {
  std::vector<double> values;

  for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    values.push_back(m_image->getValue(pixel_coord.m_x, pixel_coord.m_y));
  }

  source.setProperty<DetectionFramePixelValues>(std::move(values));
}

DetectionFramePixelValuesTask::DetectionFramePixelValuesTask(std::shared_ptr<DetectionImage> image) : m_image(image) {
}

} // SEImplementation namespace



