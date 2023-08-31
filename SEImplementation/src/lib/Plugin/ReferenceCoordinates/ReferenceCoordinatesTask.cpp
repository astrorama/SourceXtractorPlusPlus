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

#include "SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinatesTask.h"

#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinates.h"

namespace SourceXtractor {

void ReferenceCoordinatesTask::computeProperties(SourceInterface& source) const {

  std::shared_ptr<CoordinateSystem> ref_coords;

  // Either detection or first measurement image
  try {
    ref_coords = source.getProperty<DetectionFrameCoordinates>().getCoordinateSystem();
  }
  catch (PropertyNotFoundException&) {
    ref_coords = source.getProperty<MeasurementFrameCoordinates>(0).getCoordinateSystem();
  }

  source.setProperty<ReferenceCoordinates>(ref_coords);
}

} // SEImplementation namespace

