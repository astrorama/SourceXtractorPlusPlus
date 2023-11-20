/** Copyright © 2019-2023 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEImplementation/Grouping/GroupingFactory.h"

namespace SourceXtractor {

GroupingFactory::GroupingFactory(std::shared_ptr<SourceGroupFactory> source_group_factory)
  : m_source_group_factory(source_group_factory), m_hard_limit(0) {}

void GroupingFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<GroupingConfig>();
}

void GroupingFactory::configure(Euclid::Configuration::ConfigManager& manager)  {
  auto grouping_config = manager.getConfiguration<GroupingConfig>();
  m_algorithm = grouping_config.getAlgorithmOption();
  switch (m_algorithm) {
    case GroupingConfig::Algorithm::NO_GROUPING:
      m_grouping_criteria = std::make_shared<NoGroupingCriteria>();
      break;
    case GroupingConfig::Algorithm::OVERLAPPING:
      m_grouping_criteria = std::make_shared<OverlappingBoundariesCriteria>();
      break;
    case GroupingConfig::Algorithm::SPLIT_SOURCES:
      m_grouping_criteria = std::make_shared<SplitSourcesCriteria>();
      break;
    case GroupingConfig::Algorithm::MOFFAT:
      m_grouping_criteria = std::make_shared<MoffatCriteria>(grouping_config.getMoffatThreshold(), grouping_config.getMoffatMaxDistance());
      break;
    case GroupingConfig::Algorithm::ASSOC:
      m_grouping_criteria = std::make_shared<AssocCriteria>();
      break;
  }
  m_hard_limit = grouping_config.getHardLimit();
}

std::shared_ptr<SourceGroupingInterface> GroupingFactory::createGrouping() const {
  assert(m_grouping_criteria != nullptr);
  assert(m_source_group_factory != nullptr);

  // return optimized grouping if available, if not uses general grouping with criteria
  switch (m_algorithm) {
    case GroupingConfig::Algorithm::SPLIT_SOURCES:
      return std::make_shared<SplitSourcesGrouping>(m_source_group_factory, m_hard_limit);
    default:
      return std::make_shared<SourceGrouping>(m_grouping_criteria, m_source_group_factory, m_hard_limit);
  }
}

} // SourceXtractor namespace

