/*
 * MoffatGrouping.h
 *
 *  Created on: Nov 22, 2023
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_MOFFATGROUPING_H_
#define _SEIMPLEMENTATION_GROUPING_MOFFATGROUPING_H_

#include "SEFramework/Pipeline/SourceGrouping.h"

#include <map>

namespace SourceXtractor {

class MoffatGrouping : public SourceGroupingInterface {
public:

  MoffatGrouping(std::shared_ptr<SourceGroupFactory> group_factory, unsigned int hard_limit, float max_range);

  std::set<PropertyId> requiredProperties() const override;

  /// Handles a new Source
  void receiveSource(std::unique_ptr<SourceInterface> source) override;

  /// Handles a ProcessSourcesEvent to trigger the processing of some of the Sources stored in SourceGrouping
  void receiveProcessSignal(const ProcessSourcesEvent& event) override;

private:
  std::shared_ptr<SourceGroupFactory> m_group_factory;
  std::map<unsigned int, std::unique_ptr<SourceGroupInterface>> m_source_groups;
  unsigned int m_hard_limit;
  float m_max_range;

  struct GridCoord {
    int x, y;
  };

  std::map<GridCoord, std::vector<std::unique_ptr<SourceGroupInterface>>> m_groups;
};

}

#endif /* _SEIMPLEMENTATION_GROUPING_MOFFATGROUPING_H_ */
