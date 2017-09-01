/**
 * @file src/lib/Task/PixelBoundariesTask.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include <climits>

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTask.h"

namespace SExtractor {

void PixelBoundariesTask::computeProperties(SourceInterface& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getFilteredValues();
  SeFloat half_maximum = source.getProperty<PeakValue>().getMaxValue() / 2.0;

  int min_x = INT_MAX;
  int min_y = INT_MAX;
  int max_x = INT_MIN;
  int max_y = INT_MIN;

  int min_x_half = INT_MAX;
  int min_y_half = INT_MAX;
  int max_x_half = INT_MIN;
  int max_y_half = INT_MIN;


  auto i = pixel_values.begin();
  for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    SeFloat value = *i++;

    min_x = std::min(min_x, pixel_coord.m_x);
    min_y = std::min(min_y, pixel_coord.m_y);
    max_x = std::max(max_x, pixel_coord.m_x);
    max_y = std::max(max_y, pixel_coord.m_y);

    if (value >= half_maximum) {
      min_x_half = std::min(min_x_half, pixel_coord.m_x);
      min_y_half = std::min(min_y_half, pixel_coord.m_y);
      max_x_half = std::max(max_x_half, pixel_coord.m_x);
      max_y_half = std::max(max_y_half, pixel_coord.m_y);
    }
  }

  source.setProperty<PixelBoundaries>(min_x, min_y, max_x, max_y);
  source.setProperty<PixelBoundariesHalfMaximum>(min_x_half, min_y_half, max_x_half, max_y_half);
}

} // SEImplementation namespace

