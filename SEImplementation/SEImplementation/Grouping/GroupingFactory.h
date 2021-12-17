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
/*
 * GroupingFactory.h
 *
 *  Created on: Jul 2, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_GROUPINGFACTORY_H_
#define _SEIMPLEMENTATION_GROUPING_GROUPINGFACTORY_H_

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Pipeline/SourceGrouping.h"
#include "SEFramework/Source/SourceGroupFactory.h"

#include "SEImplementation/Grouping/NoGroupingCriteria.h"
#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"
#include "SEImplementation/Grouping/SplitSourcesCriteria.h"
#include "SEImplementation/Grouping/MoffatCriteria.h"


#include "SEImplementation/Configuration/GroupingConfig.h"

namespace SourceXtractor {

class GroupingFactory : public Configurable {

public:

  explicit GroupingFactory(std::shared_ptr<SourceGroupFactory> source_group_factory)
    : m_source_group_factory(source_group_factory), m_hard_limit(0) {}

  virtual ~GroupingFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override {
    manager.registerConfiguration<GroupingConfig>();
  }

  void configure(Euclid::Configuration::ConfigManager& manager) override {
    auto grouping_config = manager.getConfiguration<GroupingConfig>();
    switch (grouping_config.getAlgorithmOption()) {
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
    }
    m_hard_limit = grouping_config.getHardLimit();
  }

  std::shared_ptr<SourceGrouping> createGrouping() const {
    assert(m_grouping_criteria != nullptr);
    assert(m_source_group_factory != nullptr);

    return std::make_shared<SourceGrouping>(m_grouping_criteria, m_source_group_factory, m_hard_limit);
  }

private:
  std::shared_ptr<GroupingCriteria> m_grouping_criteria;
  std::shared_ptr<SourceGroupFactory> m_source_group_factory;
  unsigned int m_hard_limit;
};

} /* namespace SourceXtractor */



#endif /* _SEIMPLEMENTATION_GROUPING_GROUPINGFACTORY_H_ */
