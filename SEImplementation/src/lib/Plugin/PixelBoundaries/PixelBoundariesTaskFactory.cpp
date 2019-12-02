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
/**
 * @file src/lib/Task/PixelBoundariesTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTask.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTaskFactory.h"

namespace SourceXtractor {

std::shared_ptr<Task> PixelBoundariesTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<PixelBoundaries>()) {
    return std::make_shared<PixelBoundariesTask>();
  } else if (property_id == PropertyId::create<PixelBoundariesHalfMaximum>()) {
    return std::make_shared<PixelBoundariesTaskHalfMaximum>();
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace

