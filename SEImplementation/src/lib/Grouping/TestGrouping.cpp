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

#include "SEImplementation/Grouping/TestGrouping.h"
#include <ElementsKernel/Logging.h>
namespace SourceXtractor {

static Elements::Logging logger = Elements::Logging::getLogger("TestGrouping");

TestGrouping::TestGrouping(std::shared_ptr<SourceGroupFactory> group_factory, unsigned int group_size)
    : m_group_factory(group_factory), m_group_size(group_size)
{
}

std::set<PropertyId> TestGrouping::requiredProperties() const {
  return {};
}

/// Handles a new Source
void TestGrouping::receiveSource(std::unique_ptr<SourceInterface> source) {
  m_total_sources_waiting++;
  logger.debug() << "Receiving source, sources in grouping: " << m_total_sources_waiting;

  if (!m_source_group) {
    m_source_group= m_group_factory->createSourceGroup();
  }

  // Add source to current group
  m_source_group->addSource(std::move(source));

  // If the group has reached the desired size, send it to processing
  if (m_source_group->size() >= m_group_size) {
    m_total_sources_waiting -= m_source_group->size();
    logger.debug() << "Group reached size " << m_group_size
                   << ", sending group to processing, sources remaining in grouping: " << m_total_sources_waiting;
    sendSource(std::move(m_source_group));
    m_source_group.reset();
  }

}

/// Handles a ProcessSourcesEvent to trigger the processing of some of the Sources stored in SourceGrouping
void TestGrouping::receiveProcessSignal(const ProcessSourcesEvent& event) {
  logger.debug() << "Received processing signal, total sources waiting in grouping: " << m_total_sources_waiting;

  // If there is a current group with sources, send it to processing
  if (m_source_group && m_source_group->size() > 0) {
    m_total_sources_waiting -= m_source_group->size();
    logger.debug() << "Sending remaining group of size " << m_source_group->size()
                   << " to processing, sources remaining in grouping: " << m_total_sources_waiting;
    sendSource(std::move(m_source_group));
    m_source_group.reset();
  }

  logger.debug() << "Processing signal handled, total sources remaining in grouping: " << m_total_sources_waiting;
}

} // SourceXtractor namespace

