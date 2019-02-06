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

namespace SExtractor {

class GroupingFactory : public Configurable {

public:

  GroupingFactory(std::shared_ptr<SourceGroupFactory> source_group_factory)
    : m_source_group_factory(source_group_factory) {}

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
      m_grouping_criteria = std::make_shared<MoffatCriteria>(grouping_config.getMoffatThreshold());
      break;
    }
  }

  std::shared_ptr<SourceGrouping> createGrouping() const {
    assert(m_grouping_criteria != nullptr);
    assert(m_source_group_factory != nullptr);

    return std::make_shared<SourceGrouping>(m_grouping_criteria, m_source_group_factory);
  }

private:
  std::shared_ptr<GroupingCriteria> m_grouping_criteria;
  std::shared_ptr<SourceGroupFactory> m_source_group_factory;
};

} /* namespace SExtractor */



#endif /* _SEIMPLEMENTATION_GROUPING_GROUPINGFACTORY_H_ */
