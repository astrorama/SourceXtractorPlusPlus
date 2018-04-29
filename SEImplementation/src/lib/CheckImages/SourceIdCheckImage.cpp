/*
 * SegmentationCheckImage.h
 *
 *  Created on: Dec 11, 2017
 *      Author: mschefer
 */
#include "SEImplementation/CheckImages/SourceIdCheckImage.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Property/SourceId.h"

namespace SExtractor {

void SourceIdCheckImage::handleMessage(const std::shared_ptr<SourceInterface>& source) {
  if (m_check_image) {

    auto coordinates = source->getProperty<PixelCoordinateList>();

    // get the ID for each (multithresholded) source
    const auto& source_id = source->getProperty<SourceId>().getSourceId();

    // iterate over the pixels and set the source-id value
    for (auto& coord : coordinates.getCoordinateList()) {
      m_check_image->setValue(coord.m_x, coord.m_y, source_id);
    }
  }
}
}
