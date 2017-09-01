/**
 * @file src/lib/Pipeline/SourceGrouping.cpp
 * @date 05/24/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/SourceGrouping.h"


namespace SExtractor {

SourceGrouping::SourceGrouping(std::unique_ptr<GroupingCriteria> grouping_criteria,
                               std::shared_ptr<TaskProvider> task_provider)
        : m_grouping_criteria(std::move(grouping_criteria)), m_task_provider(task_provider) {
}

void SourceGrouping::handleMessage(const std::shared_ptr<Source>& source) {
  // Pointer which points to the group of the source
  std::shared_ptr<SourceGroup> matched_group = nullptr;
  
  std::vector<std::list<std::shared_ptr<SourceGroup>>::iterator> groups_to_remove {};
  
  for (auto group_it = m_source_groups.begin(); group_it != m_source_groups.end(); ++group_it) {
    // Search if the source meets the grouping criteria with any of the sources in the group
    bool in_group = false;
    for (auto& s : **group_it) {
      if (m_grouping_criteria->shouldGroup(*source, s)) {
        in_group = true;
        break; // No need to check the rest of the group sources
      }
    }
    
    if (in_group) {
      if (matched_group == nullptr) {
        matched_group = *group_it;
        matched_group->addSource(source);
      } else {
        matched_group->merge(**group_it);
        groups_to_remove.emplace_back(group_it);
      }
    }
  }
  
  // If there was no group the source should be grouped in, we create a new one
  if (matched_group == nullptr) {
    matched_group = std::make_shared<SourceGroup>(m_task_provider);
    matched_group->addSource(source);
    m_source_groups.emplace_back(matched_group);
  }
  
  for (auto& group_it : groups_to_remove) {
    m_source_groups.erase(group_it);
  }
  
}

void SourceGrouping::handleMessage(const ProcessSourcesEvent& process_event) {
  std::vector<std::list<std::shared_ptr<SourceGroup>>::iterator> groups_to_process;

  // We iterate through all the SourceGroups we have
  for (auto group_it = m_source_groups.begin(); group_it != m_source_groups.end(); ++group_it) {
    // We look at its Sources and if we find at least one that needs to be processed we put it in groups_to_process
    for (auto& source : **group_it) {
      if (process_event.m_selection_criteria.mustBeProcessed(source)) {
        groups_to_process.push_back(group_it);
        break;
      }
    }
  }

  // For each SourceGroup that we put in groups_to_process,
  for (auto& group : groups_to_process) {
    // we remove it from our list of stored SourceGroups and notify our observers
    notifyObservers(*group);
    m_source_groups.erase(group);
  }
}

} // SEFramework namespace



