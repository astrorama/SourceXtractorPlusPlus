/**
 * @file src/lib/Grouping/OverlappingBoundariesSourceList.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include <climits>

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Grouping/OverlappingBoundariesSourceList.h"

using namespace SEUtils;

namespace SEImplementation {

OverlappingBoundariesSourceList::OverlappingBoundariesSourceList(std::vector<std::shared_ptr<Source>> sources)
    : SourceList(std::move(sources)),
      m_min_coord(INT_MAX, INT_MAX),
      m_max_coord(INT_MIN, INT_MIN) {
}

void OverlappingBoundariesSourceList::addSource(std::shared_ptr<Source> source) {
  SourceList::addSource(source);
  processSource(*source);
}

void OverlappingBoundariesSourceList::removeSource(std::shared_ptr<Source> source) {
  SourceList::removeSource(source);

  m_min_coord = PixelCoordinate(INT_MAX, INT_MAX);
  m_max_coord = PixelCoordinate(INT_MIN, INT_MIN);
  for (auto& source : m_sources) {
    processSource(*source);
  }
}

void OverlappingBoundariesSourceList::merge(SourceList& source_list) {
  SourceList::merge(source_list);

  auto& sl = dynamic_cast<const OverlappingBoundariesSourceList&>(source_list);
  m_min_coord = PixelCoordinate(std::min(m_min_coord.m_x, sl.m_min_coord.m_x),
      std::min(m_min_coord.m_y, sl.m_min_coord.m_y));
  m_max_coord = PixelCoordinate(std::max(m_max_coord.m_x, sl.m_max_coord.m_x),
      std::max(m_max_coord.m_y, sl.m_max_coord.m_y));
}

void OverlappingBoundariesSourceList::processSource(Source& source) {
  auto& boundaries = source.getProperty<PixelBoundaries>();

  m_min_coord = PixelCoordinate(std::min(m_min_coord.m_x, boundaries.getMin().m_x),
      std::min(m_min_coord.m_y, boundaries.getMin().m_y));
  m_max_coord = PixelCoordinate(std::max(m_max_coord.m_x, boundaries.getMax().m_x),
      std::max(m_max_coord.m_y, boundaries.getMax().m_y));
}


} // SEImplementation namespace



