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

namespace SExtractor {

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

} // SExtractor namespace


