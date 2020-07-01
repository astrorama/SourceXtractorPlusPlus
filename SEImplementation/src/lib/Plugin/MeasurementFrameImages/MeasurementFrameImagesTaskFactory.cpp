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


#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImagesTask.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImagesTaskFactory.h"

using namespace Euclid::Configuration;

namespace SourceXtractor {

std::shared_ptr<Task> MeasurementFrameImagesTaskFactory::createTask(const PropertyId& property_id) const {

  if (property_id == PropertyId::create<MeasurementFrameImages>()) {
    return std::make_shared<MeasurementFrameImagesTask>();
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace




