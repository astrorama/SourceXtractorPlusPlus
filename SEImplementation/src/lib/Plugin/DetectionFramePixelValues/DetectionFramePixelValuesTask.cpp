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
 * @file src/lib/Task/DetectionFramePixelValuesTask.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include <memory>

#include "SEFramework/Image/ImageAccessor.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTask.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

namespace SourceXtractor {

void DetectionFramePixelValuesTask::computeProperties(SourceInterface& source) const {
  const auto& stamp = source.getProperty<DetectionFrameSourceStamp>();

  ImageAccessor<DetectionImage::PixelType> detection_image(stamp.getStamp());
  ImageAccessor<DetectionImage::PixelType> filtered_image(stamp.getFilteredStamp());
  ImageAccessor<WeightImage::PixelType>    variance_map(stamp.getVarianceStamp());

  auto offset = stamp.getTopLeft();

  std::vector<DetectionImage::PixelType> values, filtered_values;
  std::vector<WeightImage::PixelType>    variances;
  for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    auto offset_coord = pixel_coord - offset;
    values.push_back(detection_image.getValue(offset_coord.m_x, offset_coord.m_y));
    filtered_values.push_back(filtered_image.getValue(offset_coord.m_x, offset_coord.m_y));
    variances.push_back(variance_map.getValue(offset_coord.m_x, offset_coord.m_y));
  }

  source.setProperty<DetectionFramePixelValues>(std::move(values), std::move(filtered_values), std::move(variances));
}

}  // namespace SourceXtractor
