/**
 * @file src/lib/Pipeline/Deblending.cpp
 * @date 05/26/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Deblending.h"

namespace SExtractor {

Deblending::Deblending(std::vector<std::shared_ptr<DeblendAction>> actions, std::shared_ptr<TaskRegistry> task_registry)
  : m_actions(std::move(actions)),
    m_task_registry(task_registry) {
}

void Deblending::handleMessage(const std::shared_ptr<SourceList>& source_list) {
  for (auto& action : m_actions) {
    action->deblend(*source_list);
  }

  auto source_group = EntangledSourceGroup::create(source_list->getSources(), m_task_registry);
  notifyObservers(source_group);
}

} // SEFramework namespace
