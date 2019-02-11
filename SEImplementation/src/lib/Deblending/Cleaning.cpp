/*
 * Cleaning.cpp
 *
 *  Created on: 2018 M12 18
 *      Author: mschefer
 */

#include <vector>

#include "ModelFitting/utils.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FlattenedMoffatComponent.h"

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

#include "SEImplementation/Deblending/Cleaning.h"

namespace SExtractor {

using namespace ModelFitting;

void Cleaning::deblend(SourceGroupInterface& group) const {

  // FIXME why don't we have count in SourceGroupInterface?
  int count = 0;
  for (auto it = group.begin(); it != group.end(); ++it) {
    count++;
  }
  if (count <= 1) {
    return;
  }

  std::vector<SourceGroupInterface::iterator> to_be_removed;
  for (auto it = group.begin(); it != group.end(); ++it) {
    if (shouldClean(*it, group)) {
      to_be_removed.push_back(it);
    }
  }
  for (auto it : to_be_removed) {
    group.removeSource(it);
  }
  //FIXME we should also reassign pixels
}

bool Cleaning::shouldClean(SourceInterface& source, SourceGroupInterface& group) const {
  const auto& pixel_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  std::vector<double> group_influence(pixel_list.size());
  std::fill(group_influence.begin(), group_influence.end(), 0);

  for (auto it = group.begin(); it != group.end(); ++it) {
    if (&(*it) == &source) {
      continue;
    }

    auto model = createMoffatModel(*it);
    int i = 0;
    for (auto pixel : pixel_list) {
      group_influence[i++] += model.getValue(pixel.m_x, pixel.m_y);
    }
  }

  int still_valid_pixels = 0;
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getFilteredValues();
  int i = 0;
  for (auto value : pixel_values) {
    if (value > group_influence[i++]) {
      still_valid_pixels++;
    }
  }

  // FIXME should that be related to the minimum number of pixels for a detection? a new parameter?
  return still_valid_pixels <= 0;
}

// FIXME duplicated from MoffatGroupingCriteria, refactor
ExtendedModel Cleaning::createMoffatModel(SourceInterface& source) const {
  auto& model = source.getProperty<MoffatModelFitting>();

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

  // Moffat model
  std::vector<std::unique_ptr<ModelComponent>> component_list {};
  auto moff = make_unique<FlattenedMoffatComponent>(moffat_i0, moffat_index, minkowski_exponent, flat_top_offset);
  component_list.clear();
  component_list.emplace_back(std::move(moff));

  return ExtendedModel(std::move(component_list), x_scale, y_scale, moffat_rotation, size, size, x, y);
}

}

