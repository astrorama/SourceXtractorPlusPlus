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
 * GroupIdCheckImage.cpp
 *
 *  Created on: 2019 M01 30
 *      Author: mschefer
 */

#include "SEFramework/Source/SourceGroupInterface.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/GroupInfo/GroupInfo.h"

#include "SEImplementation/CheckImages/GroupIdCheckImage.h"

namespace SourceXtractor {

void GroupIdCheckImage::handleMessage(const std::shared_ptr<SourceGroupInterface>& group) {
  if (m_check_image) {
    // get the ID of the group
    auto group_id = group->getProperty<GroupInfo>().getGroupId();

    CheckImages::getInstance().lock();

    for (auto& source : *group) {
      auto& coordinates = source.getProperty<PixelCoordinateList>();

      // iterate over the pixels and set the group_id value
      for (auto& coord : coordinates.getCoordinateList()) {
        m_check_image->setValue(coord.m_x, coord.m_y, group_id);
      }
    }

    CheckImages::getInstance().unlock();
  }
}

}

