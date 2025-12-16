/** Copyright © 2019 - 2025 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEIMPLEMENTATION_GROUPING_TESTGROUPING_H_
#define _SEIMPLEMENTATION_GROUPING_TESTGROUPING_H_

#include "SEFramework/Pipeline/SourceGrouping.h"

namespace SourceXtractor {

// For test, just group sources in groups of a fixed size
class TestGrouping : public SourceGroupingInterface {
public:

  TestGrouping(std::shared_ptr<SourceGroupFactory> group_factory, unsigned int group_size);
  virtual ~TestGrouping() = default;

  std::set<PropertyId> requiredProperties() const override;

  /// Handles a new Source
  void receiveSource(std::unique_ptr<SourceInterface> source) override;

  /// Handles a ProcessSourcesEvent to trigger the processing of some of the Sources stored in SourceGrouping
  void receiveProcessSignal(const ProcessSourcesEvent& event) override;

private:
  std::shared_ptr<SourceGroupFactory> m_group_factory;
  std::unique_ptr<SourceGroupInterface> m_source_group;
  unsigned int m_group_size = 3;
  unsigned int m_total_sources_waiting = 0;
};

}

#endif /* _SEIMPLEMENTATION_GROUPING_TESTGROUPING_H_ */
