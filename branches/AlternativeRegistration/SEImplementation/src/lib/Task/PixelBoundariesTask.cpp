/**
 * @file src/lib/Task/PixelBoundariesTask.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include <climits>

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Task/PixelBoundariesTask.h"

namespace SExtractor {

void PixelBoundariesTask::computeProperties(PixelSourceInterface& source) const {
  int min_x = INT_MAX;
  int min_y = INT_MAX;
  int max_x = INT_MIN;
  int max_y = INT_MIN;

  for (auto pixel_coord : source.getPixels()) {
    min_x = std::min(min_x, pixel_coord.m_x);
    min_y = std::min(min_y, pixel_coord.m_y);
    max_x = std::max(max_x, pixel_coord.m_x);
    max_y = std::max(max_y, pixel_coord.m_y);
  }

  source.setProperty(std::unique_ptr<PixelBoundaries>(new PixelBoundaries(min_x, min_y, max_x, max_y)));
}

} // SEImplementation namespace

