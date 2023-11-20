/** Copyright © 2019 - 2023 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEImplementation/Grouping/SplitSourcesGrouping.h"
#include "SEImplementation/Property/SourceId.h"

namespace SourceXtractor {

SplitSourcesGrouping::SplitSourcesGrouping(std::shared_ptr<SourceGroupFactory> group_factory, unsigned int hard_limit)
    : m_group_factory(group_factory), m_hard_limit(hard_limit)
{
}

std::set<PropertyId> SplitSourcesGrouping::requiredProperties() const {
  return {};
}

/// Handles a new Source
void SplitSourcesGrouping::receiveSource(std::unique_ptr<SourceInterface> source) {
  auto source_id = source->getProperty<SourceId>().getDetectionId();

  if (m_source_groups.find(source_id) == m_source_groups.end()) {
    auto new_group = m_group_factory->createSourceGroup();
    m_source_groups[source_id] = std::move(new_group);
  }

  m_source_groups.at(source_id)->addSource(std::move(source));

  // TODO handle hard limit
  //    if (m_hard_limit > 0) {
}

/// Handles a ProcessSourcesEvent to trigger the processing of some of the Sources stored in SourceGrouping
void SplitSourcesGrouping::receiveProcessSignal(const ProcessSourcesEvent& event) {
  std::vector<unsigned int> groups_to_process;

  // We iterate through all the SourceGroups we have
  for (auto const& it : m_source_groups) {
    // We look at its Sources and if we find at least one that needs to be processed
    for (auto& source : *it.second) {
      if (event.m_selection_criteria->mustBeProcessed(source)) {
        groups_to_process.push_back(it.first);
        break;
      }
    }
  }

  // For each SourceGroup that we put in groups_to_process,
  for (auto group_id : groups_to_process) {
    // we remove it from our list of stored SourceGroups and notify our observers
    sendSource(std::move(m_source_groups[group_id]));
    m_source_groups.erase(group_id);
  }
}

} // SourceXtractor namespace

