/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEImplementation/Plugin/AssocMode/AssocMode.h"
#include "SEImplementation/Plugin/AssocMode/AssocModePartitionStep.h"

namespace SourceXtractor {

AssocModePartitionStep::AssocModePartitionStep(bool match_required) : m_match_required(match_required) {
}

std::vector<std::shared_ptr<SourceInterface>> AssocModePartitionStep::partition(std::shared_ptr<SourceInterface> source) const {
  if (source->getProperty<AssocMode>().getMatch() ^ !m_match_required) {
    return { source };
  } else {
    return {};
  }
}

} // SourceXtractor namespace

