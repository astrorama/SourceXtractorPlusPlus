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

void Deblending::handleMessage(const std::shared_ptr<SourceList>& source_list) {
  // Applies every DeblendAction to the SourceList
  for (auto& action : m_actions) {
    action->deblend(*source_list);
  }

  // If the SourceList still contains sources, we create an EntangledSourceGroup with them and notify the observers
  auto& sources = source_list->getSources();
  if (sources.size() > 0) {
    auto source_group = std::make_shared<SourceGroup>(m_task_provider, sources);
    notifyObservers(source_group);
  }
}

} // SEFramework namespace
