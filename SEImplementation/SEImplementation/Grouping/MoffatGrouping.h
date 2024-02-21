/** Copyright © 2019-2024 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
