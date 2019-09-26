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


