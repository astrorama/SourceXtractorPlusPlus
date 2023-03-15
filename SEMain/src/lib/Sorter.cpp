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

#include "SEMain/Sorter.h"
#include <SEImplementation/Plugin/SourceIDs/SourceID.h>
#include <algorithm>
#include <vector>

namespace SourceXtractor {

static unsigned int extractSourceId(const SourceInterface &i) {
  return i.getProperty<SourceID>().getId();
}

Sorter::Sorter(): m_output_next{1} {
}

void Sorter::receiveSource(std::unique_ptr<SourceGroupInterface> message) {
  std::vector<unsigned int> source_ids(message->size());
  std::transform(message->cbegin(), message->cend(), source_ids.begin(), extractSourceId);
  std::sort(source_ids.begin(), source_ids.end());

  auto first_source_id = source_ids.front();
  m_output_buffer.emplace(first_source_id, std::move(message));

  while (!m_output_buffer.empty() && m_output_buffer.begin()->first == m_output_next) {
    auto &next_group = m_output_buffer.begin()->second;
    m_output_next += next_group->size();
    sendSource(std::move(next_group));
    m_output_buffer.erase(m_output_buffer.begin());
  }
}
void Sorter::receiveProcessSignal(const ProcessSourcesEvent& event) {
  sendProcessSignal(event);
}

} // end SourceXtractor
