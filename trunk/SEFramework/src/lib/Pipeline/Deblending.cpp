/**
 * @file src/lib/Pipeline/Deblending.cpp
 * @date 05/26/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Deblending.h"

namespace SExtractor {

Deblending::Deblending(std::vector<std::shared_ptr<DeblendStep>> deblend_steps)
  : m_deblend_steps(std::move(deblend_steps)) {
}

void Deblending::handleMessage(const std::shared_ptr<SourceGroupInterface>& group) {
  
  // Applies every DeblendStep to the SourceGroup
  for (auto& step : m_deblend_steps) {
    step->deblend(*group);
  }

  // If the SourceGroup still contains sources, we notify the observers
  if (group->begin() != group->end()) {
    notifyObservers(group);
  }
}

} // SEFramework namespace
