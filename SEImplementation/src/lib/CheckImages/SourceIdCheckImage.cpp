/*
 * SegmentationCheckImage.h
 *
 *  Created on: Dec 11, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"

#include "SEImplementation/CheckImages/SourceIdCheckImage.h"

namespace SExtractor {

void SourceIdCheckImage::handleMessage(const std::shared_ptr<SourceGroupInterface>& group) {
  if (m_check_image) {
    for (auto& source : *group) {
      auto coordinates = source.getProperty<PixelCoordinateList>();

      // get the ID for each (multithresholded) source
      const auto& source_id = source.getProperty<SourceID>().getId();

      // iterate over the pixels and set the source-id value
      for (auto& coord : coordinates.getCoordinateList()) {
        m_check_image->setValue(coord.m_x, coord.m_y, source_id);
      }
    }
  }
}

}
