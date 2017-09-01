/* 
 * @file DeblendStepConfig.cpp
 * @author nikoapos
 */

#include "SEImplementation/Configuration/DeblendStepConfg.h"

namespace SExtractor {

DeblendStepConfg::DeblendStepConfg(long manager_id) : Configuration(manager_id) {
}

void DeblendStepConfg::addDeblendStepCreator(DeblendStepCreator step_creator) {
  if (getCurrentState() >= State::FINAL) {
    throw Elements::Exception() << "addDeblendStepCreator() call on finalized DeblendStepConfg";
  }
  m_deblend_step_creators.emplace_back(std::move(step_creator));
}

std::vector<std::shared_ptr<DeblendStep>> DeblendStepConfg::getSteps(std::shared_ptr<SourceFactory> source_factory) const {
  if (getCurrentState() < State::FINAL) {
    throw Elements::Exception() << "getSteps() call on not finalized DeblendStepConfg";
  }
  std::vector<std::shared_ptr<DeblendStep>> result;
  for (auto& creator : m_deblend_step_creators) {
    result.emplace_back(creator(source_factory));
  }
  return result;
}

} /* namespace SExtractor */