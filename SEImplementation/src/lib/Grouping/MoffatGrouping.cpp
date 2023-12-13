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

#include "SEUtils/QuadTree.h"

#include "SEImplementation/Grouping/MoffatGrouping.h"

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelEvaluator.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

namespace SourceXtractor {

MoffatGrouping::MoffatGrouping(
    std::shared_ptr<SourceGroupFactory> group_factory, unsigned int hard_limit, float max_range)
    : m_group_factory(group_factory), m_hard_limit(hard_limit), m_max_range(max_range)
{
}

std::set<PropertyId> MoffatGrouping::requiredProperties() const {
  return {
    PropertyId::create<PixelCentroid>(),
    PropertyId::create<PeakValue>(),
    PropertyId::create<MoffatModelEvaluator>(),
  };
}

/// Handles a new Source
void MoffatGrouping::receiveSource(std::unique_ptr<SourceInterface> source) {
}

/// Handles a ProcessSourcesEvent to trigger the processing of some of the Sources stored in SourceGrouping
void MoffatGrouping::receiveProcessSignal(const ProcessSourcesEvent& event) {
}

} // SourceXtractor namespace

