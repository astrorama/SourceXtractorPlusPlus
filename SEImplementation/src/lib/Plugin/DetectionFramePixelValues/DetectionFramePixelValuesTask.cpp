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
#include <mutex>

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTask.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

namespace SExtractor {

void DetectionFramePixelValuesTask::computeProperties(SourceInterface& source) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  auto detection_image = source.getProperty<DetectionFrame>().getFrame()->getSubtractedImage();
  auto filtered_image = source.getProperty<DetectionFrame>().getFrame()->getFilteredImage();
  auto variance_map = source.getProperty<DetectionFrame>().getFrame()->getVarianceMap();

  std::vector<DetectionImage::PixelType> values, filtered_values;
  std::vector<WeightImage::PixelType> variances;
  for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    values.push_back(detection_image->getValue(pixel_coord.m_x, pixel_coord.m_y));
    filtered_values.push_back(filtered_image->getValue(pixel_coord.m_x, pixel_coord.m_y));
    variances.push_back(variance_map->getValue(pixel_coord.m_x, pixel_coord.m_y));
  }

  source.setProperty<DetectionFramePixelValues>(std::move(values), std::move(filtered_values), std::move(variances));
}

} // SEImplementation namespace

