/**
 * @file src/lib/Pipeline/PixelRefinement.cpp
 * @date 05/23/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Partition.h"

namespace SExtractor {

Partition::Partition(std::vector<std::shared_ptr<PartitionStep>> steps)
  : m_steps(std::move(steps)) {
}

void Partition::handleMessage(const std::shared_ptr<SourceInterface>& source) {
  // The input of the current step
  std::vector<std::shared_ptr<SourceInterface>> step_input_sources { source };

  // Applies all the steps
  for (const auto& step : m_steps) {
    std::vector<std::shared_ptr<SourceInterface>> step_output_sources;
    // For each Source in pour input list
    for (const auto& source : step_input_sources) {
      // applies the current step
     const auto partition_output = step->partition(source);
     // then merges the result
      step_output_sources.insert(step_output_sources.end(), partition_output.begin(), partition_output.end());
    }

    // the output of that step is then used as the input of the next
    step_input_sources = std::move(step_output_sources);
  }

  // Observers are then notified of the output of the last step
  for (const auto& source : step_input_sources) {
    notifyObservers(source);
  }
}



} // SEFramework namespace



