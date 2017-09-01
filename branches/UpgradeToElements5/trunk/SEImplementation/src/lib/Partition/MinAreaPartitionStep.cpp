/**
 * @file src/lib/MinAreaPartitionStep.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Partition/MinAreaPartitionStep.h"

namespace SExtractor {

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

