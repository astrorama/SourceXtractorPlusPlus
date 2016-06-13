/**
 * @file src/lib/Grouping/OverlappingBoundariesCriteria.cpp
 * @date 06/07/16
 * @author mschefer
 */

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"
#include "SEImplementation/Grouping/OverlappingBoundariesSourceList.h"

namespace SExtractor {

bool OverlappingBoundariesCriteria::shouldGroup(const SourceList& source_list, const Source& source) const {
  auto& boundaries = source.getProperty<PixelBoundaries>();

  auto& sl = dynamic_cast<const OverlappingBoundariesSourceList&>(source_list);

  bool overlap_list_bounding_box = !(boundaries.getMin().m_x > sl.getMax().m_x ||
      boundaries.getMax().m_x < sl.getMin().m_x ||
      boundaries.getMin().m_y > sl.getMax().m_y ||
      boundaries.getMax().m_y < sl.getMin().m_y);

  // if we overlap the PixelSourceList's bounding box we check its individual sources for an overlap
  if (overlap_list_bounding_box) {
    for (auto& s : source_list.getSources()) {
      auto& other_boundaries = s->getProperty<PixelBoundaries>();
      if ( !(boundaries.getMin().m_x > other_boundaries.getMax().m_x ||
          boundaries.getMax().m_x < other_boundaries.getMin().m_x ||
          boundaries.getMin().m_y > other_boundaries.getMax().m_y ||
          boundaries.getMax().m_y < other_boundaries.getMin().m_y)) {
        return true;
      }
    }
  }

  return false;
}


} // SEImplementation namespace



