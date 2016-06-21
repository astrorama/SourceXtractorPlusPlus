/**
 * @file src/lib/Grouping/OverlappingBoundariesSourceList.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include <climits>

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Grouping/OverlappingBoundariesSourceList.h"

namespace SExtractor {

OverlappingBoundariesSourceList::OverlappingBoundariesSourceList(const std::vector<std::shared_ptr<Source>>& sources)
    : SourceList(std::move(sources)),
      m_min_coord(INT_MAX, INT_MAX),
      m_max_coord(INT_MIN, INT_MIN) {
}

void OverlappingBoundariesSourceList::addSource(std::shared_ptr<Source> source) {
  // First calls parent implementation
  SourceList::addSource(source);
  // Then processes the Source
  processSource(*source);
}

void OverlappingBoundariesSourceList::removeSource(std::shared_ptr<Source> source) {
  // Calls parent implementation
  SourceList::removeSource(source);

  // TODO: optimize by only recomputing if the source's bounding box shares a border with the overall bounding box

  // Resets the bounding box coordinates
  m_min_coord = PixelCoordinate(INT_MAX, INT_MAX);
  m_max_coord = PixelCoordinate(INT_MIN, INT_MIN);

  // And redo the computation by processing all sources
  for (auto& source : m_sources) {
    processSource(*source);
  }
}

void OverlappingBoundariesSourceList::merge(const SourceList& source_list) {
  // Calls parent implementation
  SourceList::merge(source_list);

  // We are expecting to be merged with an other OverlappingBoundariesSourceList
  // Convert the reference to the appropriate type.
  auto& sl = dynamic_cast<const OverlappingBoundariesSourceList&>(source_list);

  // Merge the two bounding boxes directly
  m_min_coord = PixelCoordinate(std::min(m_min_coord.m_x, sl.m_min_coord.m_x),
      std::min(m_min_coord.m_y, sl.m_min_coord.m_y));
  m_max_coord = PixelCoordinate(std::max(m_max_coord.m_x, sl.m_max_coord.m_x),
      std::max(m_max_coord.m_y, sl.m_max_coord.m_y));
}

void OverlappingBoundariesSourceList::processSource(const Source& source) {
  auto& boundaries = source.getProperty<PixelBoundaries>();

  m_min_coord = PixelCoordinate(std::min(m_min_coord.m_x, boundaries.getMin().m_x),
      std::min(m_min_coord.m_y, boundaries.getMin().m_y));
  m_max_coord = PixelCoordinate(std::max(m_max_coord.m_x, boundaries.getMax().m_x),
      std::max(m_max_coord.m_y, boundaries.getMax().m_y));
}


} // SEImplementation namespace



