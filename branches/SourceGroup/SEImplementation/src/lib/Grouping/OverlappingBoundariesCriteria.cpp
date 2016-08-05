/**
 * @file src/lib/Grouping/OverlappingBoundariesCriteria.cpp
 * @date 06/07/16
 * @author mschefer
 */

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"

namespace SExtractor {

bool OverlappingBoundariesCriteria::shouldGroup(const SourceInterface& first, const SourceInterface& second) const {
  auto& first_boundaries = first.getProperty<PixelBoundaries>();
  auto& second_boundaries = second.getProperty<PixelBoundaries>();
  return !(first_boundaries.getMin().m_x > second_boundaries.getMax().m_x ||
          first_boundaries.getMax().m_x < second_boundaries.getMin().m_x ||
          first_boundaries.getMin().m_y > second_boundaries.getMax().m_y ||
          first_boundaries.getMax().m_y < second_boundaries.getMin().m_y);
}


} // SEImplementation namespace



