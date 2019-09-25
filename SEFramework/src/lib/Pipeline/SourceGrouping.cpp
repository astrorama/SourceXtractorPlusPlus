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
/**
 * @file src/lib/Pipeline/SourceGrouping.cpp
 * @date 05/24/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/SourceGrouping.h"


namespace SExtractor {

SourceGrouping::SourceGrouping(std::shared_ptr<GroupingCriteria> grouping_criteria,
                               std::shared_ptr<SourceGroupFactory> group_factory)
        : m_grouping_criteria(grouping_criteria), m_group_factory(group_factory) {
}

void SourceGrouping::handleMessage(const std::shared_ptr<SourceInterface>& source) {
  // Pointer which points to the group of the source
  std::shared_ptr<SourceGroupInterface> matched_group = nullptr;
  
  std::vector<std::list<std::shared_ptr<SourceGroupInterface>>::iterator> groups_to_remove {};
  
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
    matched_group = m_group_factory->createSourceGroup();
    matched_group->addSource(source);
    m_source_groups.emplace_back(matched_group);
  }
  
  for (auto& group_it : groups_to_remove) {
    m_source_groups.erase(group_it);
  }
}

void SourceGrouping::handleMessage(const ProcessSourcesEvent& process_event) {
  std::vector<std::list<std::shared_ptr<SourceGroupInterface>>::iterator> groups_to_process;

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



