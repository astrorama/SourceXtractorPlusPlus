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
/* 
 * @file PartitionStepConfig.cpp
 * @author nikoapos
 */

#include "SEImplementation/Configuration/PartitionStepConfig.h"

namespace SExtractor {

PartitionStepConfig::PartitionStepConfig(long manager_id) : Configuration(manager_id) {
}

void PartitionStepConfig::addPartitionStepCreator(PartitionStepCreator step_creator) {
  if (getCurrentState() >= State::FINAL) {
    throw Elements::Exception() << "addPartitionStep() call on finalized PartitionStepConfig";
  }
  m_partition_step_creators.emplace_back(std::move(step_creator));
}

std::vector<std::shared_ptr<PartitionStep>> PartitionStepConfig::getSteps(std::shared_ptr<SourceFactory> source_factory) const {
  if (getCurrentState() < State::FINAL) {
    throw Elements::Exception() << "getSteps() call on not finalized PartitionStepConfig";
  }
  std::vector<std::shared_ptr<PartitionStep>> result;
  for (auto& creator : m_partition_step_creators) {
    result.emplace_back(creator(source_factory));
  }
  return result;
}

} /* namespace SExtractor */