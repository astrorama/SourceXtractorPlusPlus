/*
 * MoffatCriteria.cpp
 *
 *  Created on: 2019 M01 25
 *      Author: mschefer
 */

#include "SEImplementation/Grouping/MoffatCriteria.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

#include "ModelFitting/utils.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FlattenedMoffatComponent.h"

namespace SExtractor {

using namespace ModelFitting;

bool MoffatCriteria::doesImpact(const MoffatModelFitting& model, const SourceInterface& source) const {
  if (model.getIterations() == 0) {
    return false;
  }

  ManualParameter x(model.getX());
  ManualParameter y(model.getY());
  ManualParameter moffat_i0(model.getMoffatI0());
  ManualParameter moffat_index(model.getMoffatIndex());
  ManualParameter minkowski_exponent(model.getMinkowksiExponent());
  ManualParameter flat_top_offset(model.getTopOffset());
  double size = model.getSize();
  ManualParameter x_scale(model.getXScale());
  ManualParameter y_scale(model.getYScale());
  ManualParameter moffat_rotation(model.getMoffatRotation());

  auto& centroid = source.getProperty<PixelCentroid>();
  auto max_value = source.getProperty<PeakValue>().getMaxValue();

  // Moffat model
  std::vector<std::unique_ptr<ModelComponent>> component_list {};
  auto moff = make_unique<FlattenedMoffatComponent>(moffat_i0, moffat_index, minkowski_exponent, flat_top_offset);
  component_list.clear();
  component_list.emplace_back(std::move(moff));
  ExtendedModel extended_model(std::move(component_list), x_scale, y_scale, moffat_rotation, size, size, x, y);

  double model_value = extended_model.getValue(centroid.getCentroidX(), centroid.getCentroidY());
//  if ((model_value / max_value) > m_threshold) {
//    std::cout << model.getIterations() << " " << x.getValue() << " " << y.getValue() << " " << centroid.getCentroidX() << " " << centroid.getCentroidY() <<" " << model_value << " " << max_value << "\n";
//  }

  return (model_value / max_value) > m_threshold;
}

bool MoffatCriteria::shouldGroup(const SourceInterface& first, const SourceInterface& second) const {
  auto& first_moffat_model = first.getProperty<MoffatModelFitting>();
  auto& second_moffat_model = second.getProperty<MoffatModelFitting>();

  return doesImpact(first_moffat_model, second) || doesImpact(second_moffat_model, first);
}

} // SExtractor namespace


