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

namespace SExtractor {

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

