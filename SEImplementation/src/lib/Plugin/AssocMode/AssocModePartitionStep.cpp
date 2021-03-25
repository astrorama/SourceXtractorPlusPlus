/*
 * AssocPartitionStep.cpp
 *
 *  Created on: Mar 18, 2021
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/AssocMode/AssocMode.h"
#include "SEImplementation/Plugin/AssocMode/AssocModePartitionStep.h"

namespace SourceXtractor {

AssocModePartitionStep::AssocModePartitionStep(bool match_required) : m_match_required(match_required) {
}

std::vector<std::shared_ptr<SourceInterface>> AssocModePartitionStep::partition(std::shared_ptr<SourceInterface> source) const {
  if (source->getProperty<AssocMode>().getMatch() ^ !m_match_required) {
    return { source };
  } else {
    return {};
  }
}

} // SourceXtractor namespace

