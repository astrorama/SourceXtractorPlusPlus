/**
 * @file src/lib/Pipeline/SourceGrouping.cpp
 * @date 05/24/16
 * @author mschefer
 */

#include "SEFramework/Source/SourceList.h"

#include "SEFramework/Pipeline/SourceGrouping.h"
#include "SEFramework/Source/SourceGroup.h"


namespace SExtractor {

SourceGrouping::SourceGrouping(std::unique_ptr<GroupingCriteria> grouping_criteria,
    SourceList::SourceListFactory source_list_factory, std::shared_ptr<TaskProvider> task_provider)
    : m_grouping_criteria(std::move(grouping_criteria)), m_source_list_factory(source_list_factory),
      m_task_provider(task_provider) {
}

void SourceGrouping::handleMessage(const std::shared_ptr<Source>& source) {
  // Makes a list of all currently existing SourceLists that this source should be grouped with
  std::list<std::shared_ptr<SourceList>> lists_to_merge;
  for (auto& list : m_source_lists) {
    for (auto& list_source : list->getSources()) {
      if (m_grouping_criteria->shouldGroup(*source, * list_source)) {
        lists_to_merge.push_back(list);
        break;
      }
    }
  }

  if (lists_to_merge.empty()) {
    // If we didn't find any SourceList to group this source into, we just create a new one and put it in there
    auto new_list = std::shared_ptr<SourceList>(m_source_list_factory({}));
    new_list->addSource(source);
    m_source_lists.push_back(new_list);
  } else {
    // We found at least one SourceList, we take the first one and put the Source into it
    auto main_list = lists_to_merge.front();
    lists_to_merge.pop_front();
    main_list->addSource(source);

    // If there are more SourceLists that we can group with, we merge them with the first one
    for (auto list : lists_to_merge) {
      // we have to remove the list as we'll put its Sources in the main list
      m_source_lists.remove(list);
      // merge the list with our main list
      main_list->merge(*list);
    }
  }
}

void SourceGrouping::handleMessage(const ProcessSourcesEvent& process_event) {
  std::list<std::shared_ptr<SourceList>> lists_to_process;

  // We iterate through all the SourceList we have
  for (auto& list : m_source_lists) {
    // We look at its Sources and if we find at least one that needs to be processed we put it in lists_to_process
    for (const auto& source : list->getSources()) {
      if (process_event.m_selection_criteria.mustBeProcessed(*source)) {
        lists_to_process.push_back(list);
        break;
      }
    }
  }

  // For each SourceList that we put in lists_to_process,
  for (auto& list : lists_to_process) {
    // we remove it from our list of stored SourceLists and notify our observers
    m_source_lists.remove(list);
    notifyObservers(std::make_shared<SourceGroup>(m_task_provider, list->getSources()));
  }
}

} // SEFramework namespace



