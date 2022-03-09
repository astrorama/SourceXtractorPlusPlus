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
 * SegmentationCheckImage.h
 *
 *  Created on: Dec 11, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/DetectionFrameInfo/DetectionFrameInfo.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"

#include "SEImplementation/CheckImages/SourceIdCheckImage.h"

namespace SourceXtractor {

void SourceIdCheckImage::handleMessage(const std::shared_ptr<SourceGroupInterface>& group) {
  auto hdu_index = group->cbegin()->getProperty<DetectionFrameInfo>().getHduIndex();
  auto check_image = CheckImages::getInstance().getPartitionImage(hdu_index);
  if (check_image != nullptr) {
    for (auto& source : *group) {
      auto coordinates = source.getProperty<PixelCoordinateList>();

      // get the ID for each (multithresholded) source
      const auto& source_id = source.getProperty<SourceID>().getId();

      // iterate over the pixels and set the source-id value
      for (auto& coord : coordinates.getCoordinateList()) {
        check_image->setValue(coord.m_x, coord.m_y, source_id);
      }
    }
  }
}

}
