/**
 * @file src/lib/Pipeline/Deblending.cpp
 * @date 05/26/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Deblending.h"

namespace SExtractor {

Deblending::Deblending(std::vector<std::shared_ptr<DeblendAction>> actions, std::shared_ptr<TaskProvider> task_provider)
  : m_actions(std::move(actions)),
    m_task_provider(task_provider) {
}

void Deblending::handleMessage(const std::shared_ptr<SourceGroup>& group) {
  
  // Applies every DeblendAction to the SourceGroup
  for (auto& action : m_actions) {
    action->deblend(*group);
  }

  // If the SourceGroup still contains sources, we notify the observers
  if (group->begin() != group->end()) {
    notifyObservers(group);
  }
}

} // SEFramework namespace
