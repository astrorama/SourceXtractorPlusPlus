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
 * DetectionFrameGroupStampTaskFactory.cpp
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "Configuration/ConfigManager.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampTask.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampTaskFactory.h"

using namespace Euclid::Configuration;

namespace SourceXtractor {

std::shared_ptr<Task> DetectionFrameGroupStampTaskFactory::createTask(const PropertyId& property_id) const {

  if (property_id == PropertyId::create<DetectionFrameGroupStamp>()) {
    return std::make_shared<DetectionFrameGroupStampTask>();
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace




