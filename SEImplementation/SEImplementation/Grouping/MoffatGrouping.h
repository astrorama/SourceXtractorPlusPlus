/*
 * MoffatGrouping.h
 *
 *  Created on: Nov 22, 2023
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_MOFFATGROUPING_H_
#define _SEIMPLEMENTATION_GROUPING_MOFFATGROUPING_H_

#include "SEFramework/Pipeline/SourceGrouping.h"
#include "SEUtils/QuadTree.h"

#include <map>
#include <vector>
#include <functional>

namespace SourceXtractor {

class MoffatGrouping : public SourceGroupingInterface {
public:
  struct SourceInfo {
    std::unique_ptr<SourceInterface> m_source;
    double m_x, m_y;
    size_t m_group_id;
  };

  using Group = std::vector<std::shared_ptr<SourceInfo>>;

  MoffatGrouping(std::shared_ptr<GroupingCriteria> grouping_criteria,
      std::shared_ptr<SourceGroupFactory> group_factory, unsigned int hard_limit, float max_range);
  virtual ~MoffatGrouping() = default;

  std::set<PropertyId> requiredProperties() const override;

  /// Handles a new Source
  void receiveSource(std::unique_ptr<SourceInterface> source) override;

  /// Handles a ProcessSourcesEvent to trigger the processing of some of the Sources stored in SourceGrouping
  void receiveProcessSignal(const ProcessSourcesEvent& event) override;

private:
  std::shared_ptr<GroupingCriteria> m_grouping_criteria;
  std::shared_ptr<SourceGroupFactory> m_group_factory;
  unsigned int m_hard_limit;
  float m_max_range;

  size_t m_group_counter;
  std::map<unsigned int, std::shared_ptr<Group>> m_groups;
  QuadTree<std::shared_ptr<SourceInfo>> m_tree;
};

}

#endif /* _SEIMPLEMENTATION_GROUPING_MOFFATGROUPING_H_ */
