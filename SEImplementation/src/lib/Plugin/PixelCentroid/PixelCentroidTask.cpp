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
 * @file src/lib/Task/PixelCentroidTask.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTask.h"

namespace SExtractor {

void PixelCentroidTask::computeProperties(SourceInterface& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getFilteredValues();
  const auto& min_coord = source.getProperty<PixelBoundaries>().getMin();

  double centroid_x = 0.0;
  double centroid_y = 0.0;
  double total_value = 0.0;

  auto i = pixel_values.begin();
  for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    pixel_coord -= min_coord;
    SeFloat value = *i++;

    total_value += value;
    centroid_x += pixel_coord.m_x * value;
    centroid_y += pixel_coord.m_y * value;
  }

  centroid_x /= total_value;
  centroid_y /= total_value;

  source.setProperty<PixelCentroid>(centroid_x + min_coord.m_x, centroid_y + min_coord.m_y);
}

}

