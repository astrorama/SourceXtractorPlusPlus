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