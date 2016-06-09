/**
 * @file src/lib/MinAreaPartitionStep.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include "SEImplementation/Partition/MinAreaPartitionStep.h"

namespace SEImplementation {

MinAreaPartitionStep::MinAreaPartitionStep(unsigned int min_pixel_count) : m_min_pixel_count (min_pixel_count) {
}

std::vector<std::shared_ptr<Source>> MinAreaPartitionStep::partition(std::shared_ptr<Source> source) const {
  if (source->getPixels().size() < m_min_pixel_count) {
    return {};
  } else {
    return { source };
  }
}

} // SEImplementation namespace

