/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file src/lib/Pipeline/PixelRefinement.cpp
 * @date 05/23/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Partition.h"

namespace SourceXtractor {

Partition::Partition(std::vector<std::shared_ptr<PartitionStep>> steps)
  : m_steps(std::move(steps)) {
}

void Partition::receiveSource(std::unique_ptr<SourceInterface> input_source) {
  // The input of the current step
  std::vector<std::unique_ptr<SourceInterface>> step_input_sources;
  step_input_sources.emplace_back(std::move(input_source));

  // Applies all the steps
  for (const auto& step : m_steps) {
    std::vector<std::unique_ptr<SourceInterface>> step_output_sources;
    // For each Source in pour input list
    for (auto& source : step_input_sources) {
      // applies the current step
      auto partition_output = step->partition(std::move(source));
      // then merges the result
      step_output_sources.insert(step_output_sources.end(), std::make_move_iterator(partition_output.begin()),
                                 std::make_move_iterator(partition_output.end()));
    }

    // the output of that step is then used as the input of the next
    step_input_sources = std::move(step_output_sources);
  }

  // Observers are then notified of the output of the last step
  for (auto& source : step_input_sources) {
    sendSource(std::move(source));
  }
}
void Partition::receiveProcessSignal(const ProcessSourcesEvent& event) {
  sendProcessSignal(event);
}

} // SEFramework namespace
