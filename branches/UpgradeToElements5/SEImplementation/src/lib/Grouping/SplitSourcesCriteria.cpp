/*
 * SplitSourcesCriteria.cpp
 *
 *  Created on: Apr 26, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Grouping/SplitSourcesCriteria.h"
#include "SEImplementation/Property/SourceId.h"

namespace SExtractor {

bool SplitSourcesCriteria::shouldGroup(const SourceInterface& first, const SourceInterface& second) const {
  auto first_id = first.getProperty<SourceId>().getDetectionId();
  auto second_id = second.getProperty<SourceId>().getDetectionId();

  return first_id == second_id;
}

} // SEImplementation namespace

