/**
 * @file src/lib/Task/PixelCentroidTask.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTask.h"

namespace SExtractor {

void PixelCentroidTask::computeProperties(SourceInterface& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getValues();

  SeFloat centroid_x = 0.0;
  SeFloat centroid_y = 0.0;
  SeFloat total_value = 0.0;

  auto i = pixel_values.begin();
  for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    SeFloat value = *i++;

    total_value += value;
    centroid_x += pixel_coord.m_x * value;
    centroid_y += pixel_coord.m_y * value;
  }

  centroid_x /= total_value;
  centroid_y /= total_value;

  source.setProperty<PixelCentroid>(centroid_x, centroid_y);
}


} // SEImplementation namespace



