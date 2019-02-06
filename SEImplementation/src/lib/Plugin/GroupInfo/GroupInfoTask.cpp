/*
 * GroupInfoTask.cpp
 *
 *  Created on: 2019 M01 29
 *      Author: mschefer
 */


#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/GroupInfo/GroupInfo.h"
#include "SEImplementation/Plugin/GroupInfo/GroupInfoTask.h"

namespace SExtractor {

void GroupInfoTask::computeProperties(SourceGroupInterface& group) const {
  static unsigned int group_id  = 1;
  group.setProperty<GroupInfo>(group_id++);
}

}


