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
 * @file src/lib/MinAreaPartitionStep.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Partition/MinAreaPartitionStep.h"

namespace SourceXtractor {

MinAreaPartitionStep::MinAreaPartitionStep(unsigned int min_pixel_count) : m_min_pixel_count (min_pixel_count) {
}

std::vector<std::shared_ptr<SourceInterface>> MinAreaPartitionStep::partition(std::shared_ptr<SourceInterface> source) const {
  if (source->getProperty<PixelCoordinateList>().getCoordinateList().size() < m_min_pixel_count) {
    return {};
  } else {
    return { source };
  }
}

} // SEImplementation namespace

