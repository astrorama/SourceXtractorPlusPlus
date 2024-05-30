/** Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * @file SEFramework/Pipeline/SourceGrouping.h
 * @date 05/24/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_SOURCEGROUPING_H
#define _SEFRAMEWORK_PIPELINE_SOURCEGROUPING_H

#include <list>
#include <memory>
#include <set>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Pipeline/PipelineStage.h"
#include "SEFramework/Source/SourceGroupFactory.h"
#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SourceXtractor {

/**
 * @class SelectionCriteria
 * @brief Used to determine if a Source is selected for processing.
 *
 */
class SelectionCriteria {
public:
  /**
   * @brief Destructor
   */
  virtual ~SelectionCriteria() = default;

  /// Determines if the given Source must be processed or not
  virtual bool mustBeProcessed(const SourceInterface& source) const = 0;
};

/**
 * @class SelectAllCriteria
 * @brief An implementation of SelectionCriteria that just marks all Sources as selected
 *
 */
class SelectAllCriteria : public SelectionCriteria {
public:
  bool mustBeProcessed(const SourceInterface& ) const override {
    return true;
  }
};

/**
 * @class GroupingCriteria
 * @brief Criteria used by SourceGrouping to determine if two sources should be grouped together
 *
 */
class GroupingCriteria {
public:
  /**
   * @brief Destructor
   */
  virtual ~GroupingCriteria() = default;

  /// Determines if the two sources should be grouped together
  virtual bool shouldGroup(const SourceInterface& first, const SourceInterface& second) const = 0;

  /// Return a set of used properties so they can be pre-fetched
  virtual std::set<PropertyId> requiredProperties() const { return {}; }
};

class SourceGroupingInterface : public PipelineEmitter<SourceGroupInterface>, public PipelineReceiver<SourceInterface> {
public:
  /// Returns the set of required properties to compute the grouping
  virtual std::set<PropertyId> requiredProperties() const = 0;
};

/**
 * @class SourceGrouping
 * @brief SourceGrouping takes Source, groups them together according to its GroupingCriteria and stores them.
 *  When a ProcessSourcesEvent is received the sources that need to be processed are output together with the
 *  sources they are grouped with as a SourceGroup.
 *
 */
class SourceGrouping : public SourceGroupingInterface {
public:

  /**
   * @brief Destructor
   */
  virtual ~SourceGrouping() = default;

  SourceGrouping(std::shared_ptr<GroupingCriteria> grouping_criteria,
                 std::shared_ptr<SourceGroupFactory> group_factory,
                 unsigned int hard_limit);

  /// Returns the set of required properties to compute the grouping
  std::set<PropertyId> requiredProperties() const override;

  /// Handles a new Source
  void receiveSource(std::unique_ptr<SourceInterface> source) override;

  /// Handles a ProcessSourcesEvent to trigger the processing of some of the Sources stored in SourceGrouping
  void receiveProcessSignal(const ProcessSourcesEvent& event) override;

private:
  std::shared_ptr<GroupingCriteria> m_grouping_criteria;
  std::shared_ptr<SourceGroupFactory> m_group_factory;
  std::list<std::unique_ptr<SourceGroupInterface>> m_source_groups;
  unsigned int m_hard_limit;

}; /* End of SourceGrouping class */


} /* namespace SourceXtractor */


#endif
