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
 * @file src/lib/Grouping/OverlappingBoundariesCriteria.cpp
 * @date 06/07/16
 * @author mschefer
 */

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"

namespace SourceXtractor {

bool OverlappingBoundariesCriteria::shouldGroup(const SourceInterface& first, const SourceInterface& second) const {
  auto& first_boundaries = first.getProperty<PixelBoundaries>();
  auto& second_boundaries = second.getProperty<PixelBoundaries>();
  return !(first_boundaries.getMin().m_x > second_boundaries.getMax().m_x ||
          first_boundaries.getMax().m_x < second_boundaries.getMin().m_x ||
          first_boundaries.getMin().m_y > second_boundaries.getMax().m_y ||
          first_boundaries.getMax().m_y < second_boundaries.getMin().m_y);
}


} // SourceXtractor namespace



