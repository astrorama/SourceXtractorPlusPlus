/**
 * @file src/lib/Pipeline/SourceGrouping.cpp
 * @date 05/24/16
 * @author mschefer
 */

#include "SEFramework/Source/SourceList.h"

#include "SEFramework/Pipeline/SourceGrouping.h"


namespace SEFramework {

SourceGrouping::SourceGrouping(std::unique_ptr<GroupingCriteria> grouping_criteria,
    SourceList::SourceListFactory source_list_factory)
    : m_grouping_criteria(std::move(grouping_criteria)), m_source_list_factory(source_list_factory) {
}

void SourceGrouping::handleMessage(const std::shared_ptr<Source>& source) {
    std::list<std::shared_ptr<SourceList>> lists_to_merge;
    for (auto& list : m_source_lists) {
      if (m_grouping_criteria->shouldGroup(*list, *source)) {
        lists_to_merge.push_back(list);
      }
    }

    if (lists_to_merge.empty()) {
      auto new_list = m_source_list_factory({});
      new_list->addSource(source);
      m_source_lists.push_back(new_list);
    } else {
      auto main_list = lists_to_merge.front();
      lists_to_merge.pop_front();
      main_list->addSource(source);

      for (auto list : lists_to_merge) {
        m_source_lists.remove(list);
        main_list->merge(*list);
      }
    }
}

void SourceGrouping::handleMessage(const ProcessSourcesEvent& process_event) {
  std::list<std::shared_ptr<SourceList>> lists_to_process;

  for (auto& list : m_source_lists) {
    for (const auto& source : list->getSources()) {
      if (process_event.m_selection_criteria.mustBeProcessed(*source)) {
        lists_to_process.push_back(list);
        break;
      }
    }
  }

  for (auto& list : lists_to_process) {
    m_source_lists.remove(list);
    notifyObservers(list);
  }
}



} // SEFramework namespace



