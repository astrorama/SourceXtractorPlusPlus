/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

namespace SourceXtractor {

void DetectionIdCheckImage::handleMessage(const std::shared_ptr<SourceInterface>& source) {
  auto check_image = CheckImages::getInstance().getSegmentationImage();
  if (check_image != nullptr) {
    auto coordinates = source->getProperty<PixelCoordinateList>();

    // get the ID for each detected source
    const auto& source_id = source->getProperty<SourceId>().getDetectionId();

    // iterate over the pixels and set the detection_id value
    for (auto& coord : coordinates.getCoordinateList()) {
      check_image->setValue(coord.m_x, coord.m_y, source_id);
    }
  }
}

}
