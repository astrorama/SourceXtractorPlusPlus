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
 * MoffatCriteria.cpp
 *
 *  Created on: 2019 M01 25
 *      Author: mschefer
 */

#include "SEImplementation/Grouping/MoffatCriteria.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelEvaluator.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

namespace SourceXtractor {

using namespace ModelFitting;

bool MoffatCriteria::doesImpact(const SourceInterface& impactor, const SourceInterface& impactee) const {
  auto& extended_model = impactor.getProperty<MoffatModelEvaluator>();
  if (extended_model.getIterations() == 0) {
    return false;
  }

  auto& centroid = impactee.getProperty<PixelCentroid>();
  auto& other_centroid = impactor.getProperty<PixelCentroid>();

  auto dx = centroid.getCentroidX() - other_centroid.getCentroidX();
  auto dy = centroid.getCentroidY() - other_centroid.getCentroidY();
  if (dx*dx + dy*dy > m_max_distance * m_max_distance) {
    return false;
  }

  auto max_value = impactee.getProperty<PeakValue>().getMaxValue();

  double model_value = extended_model.getValue(centroid.getCentroidX(), centroid.getCentroidY());

  return (model_value / max_value) > m_threshold;
}

bool MoffatCriteria::shouldGroup(const SourceInterface& first, const SourceInterface& second) const {
  return doesImpact(first, second) || doesImpact(second, first);
}

std::set<PropertyId> MoffatCriteria::requiredProperties() const {
  return {
    PropertyId::create<PixelCentroid>(),
    PropertyId::create<PeakValue>(),
    PropertyId::create<MoffatModelEvaluator>(),
  };
}

} // SourceXtractor namespace


