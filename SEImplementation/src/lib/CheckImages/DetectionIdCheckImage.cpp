/*
 * DetectionIdCheckImage.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: mschefer
 */

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Property/SourceId.h"

#include "SEImplementation/CheckImages/DetectionIdCheckImage.h"

namespace SExtractor {

void DetectionIdCheckImage::handleMessage(const std::shared_ptr<SourceInterface>& source) {
  if (m_check_image) {
    CheckImages::getInstance().lock();

    auto coordinates = source->getProperty<PixelCoordinateList>();

    // get the ID for each detected source
    const auto& source_id = source->getProperty<SourceId>().getDetectionId();

    // iterate over the pixels and set the detection_id value
    for (auto& coord : coordinates.getCoordinateList()) {
      m_check_image->setValue(coord.m_x, coord.m_y, source_id);
    }

    CheckImages::getInstance().unlock();
  }
}

}
