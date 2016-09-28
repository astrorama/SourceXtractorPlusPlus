/*
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include <memory>

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEFramework/SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"

namespace SExtractor {

void AperturePhotometryTask::computeProperties(SourceInterface& source) const {
  auto detection_frame = source.getProperty<DetectionFrame>().getDetectionImage();
  auto pixel_centroid = source.getProperty<PixelCentroid>();
  auto pixel_boundaries = source.getProperty<PixelBoundaries>();

  int min_x = std::max<int>(0, pixel_centroid.getCentroidX() - m_radius);
  int min_y = std::max<int>(0, pixel_centroid.getCentroidY() - m_radius);
  int max_x = std::min<int>(detection_frame->getWidth() - 1, pixel_centroid.getCentroidX() + m_radius);
  int max_y = std::min<int>(detection_frame->getHeight() - 1, pixel_centroid.getCentroidY() + m_radius);

  SeFloat flux = 0;
  for (int pixel_y = min_y; pixel_y <= max_y; pixel_y++) {
    for (int pixel_x = min_x; pixel_x <= max_x; pixel_x++) {
      auto dx = pixel_x - pixel_centroid.getCentroidX();
      auto dy = pixel_y - pixel_centroid.getCentroidY();
      if (dx * dx + dy * dy <= m_radius * m_radius) {
        auto value = detection_frame->getValue(pixel_x, pixel_y);
        flux += value;
      }
    }
  }

  source.setIndexedProperty<AperturePhotometry>(m_instance, flux);
}

}

