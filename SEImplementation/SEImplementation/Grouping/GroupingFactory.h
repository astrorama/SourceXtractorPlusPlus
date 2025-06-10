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
#include "SEImplementation/Grouping/AssocCriteria.h"

#include "SEImplementation/Configuration/GroupingConfig.h"

namespace SourceXtractor {

class GroupingFactory : public Configurable {

public:

  explicit GroupingFactory(std::shared_ptr<SourceGroupFactory> source_group_factory);

  virtual ~GroupingFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  std::shared_ptr<SourceGroupingInterface> createGrouping() const;

private:
  std::shared_ptr<GroupingCriteria> getCriteria() const;

  GroupingConfig::Algorithm m_algorithm;
  std::shared_ptr<GroupingCriteria> m_grouping_criteria;
  std::shared_ptr<SourceGroupFactory> m_source_group_factory;
  unsigned int m_hard_limit;
  double m_moffat_max_distance;
};

} /* namespace SourceXtractor */



#endif /* _SEIMPLEMENTATION_GROUPING_GROUPINGFACTORY_H_ */
