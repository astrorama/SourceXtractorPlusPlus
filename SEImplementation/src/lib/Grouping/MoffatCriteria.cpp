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
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingUtils.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

namespace SourceXtractor {

using namespace ModelFitting;

bool MoffatCriteria::doesImpact(const SourceInterface& impactor, const SourceInterface& impactee) const {
  auto& model = impactor.getProperty<MoffatModelFitting>();
  if (model.getIterations() == 0) {
    return false;
  }

  auto& centroid = impactee.getProperty<PixelCentroid>();
  auto max_value = impactee.getProperty<PeakValue>().getMaxValue();

  MoffatModelEvaluator extended_model(impactor);

  double model_value = extended_model.getValue(centroid.getCentroidX(), centroid.getCentroidY());

  return (model_value / max_value) > m_threshold;
}

bool MoffatCriteria::shouldGroup(const SourceInterface& first, const SourceInterface& second) const {
//  auto& first_moffat_model = first.getProperty<MoffatModelFitting>();
//  auto& second_moffat_model = second.getProperty<MoffatModelFitting>();

  return doesImpact(first, second) || doesImpact(second, first);
}

} // SourceXtractor namespace


