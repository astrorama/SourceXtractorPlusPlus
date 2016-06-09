/**
 * @file src/lib/Pipeline/PixelRefinement.cpp
 * @date 05/23/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Partition.h"

namespace SEFramework {

Partition::Partition(std::vector<std::shared_ptr<PartitionStep>> steps)
  : m_steps(std::move(steps)) {
}

void Partition::handleMessage(const std::shared_ptr<Source>& source) {
  std::vector<std::shared_ptr<Source>> step_input_sources { source };

  for (const auto& step : m_steps) {
    std::vector<std::shared_ptr<Source>> step_output_sources;
    for (const auto& source : step_input_sources) {
     const auto partition_output = step->partition(source);
      step_output_sources.insert(step_output_sources.end(), partition_output.begin(), partition_output.end());
    }

    step_input_sources = std::move(step_output_sources);
  }

  for (const auto& source : step_input_sources) {
    notifyObservers(source);
  }
}



} // SEFramework namespace



