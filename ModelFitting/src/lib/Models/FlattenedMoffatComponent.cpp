/*
 * FlattenedMoffatComponent.cpp
 *
 *  Created on: 2019 M01 7
 *      Author: mschefer
 */

#include <math.h>

#include "ModelFitting/Models/FlattenedMoffatComponent.h"

namespace ModelFitting {

FlattenedMoffatComponent::FlattenedMoffatComponent(BasicParameter& max_intensity, BasicParameter& moffat_index,
    BasicParameter& minkowski_distance_param, BasicParameter& flat_top_offset) :
      m_max_intensity { max_intensity.getValue() },
      m_max_intensity_updater {max_intensity, m_max_intensity},
      m_moffat_index { moffat_index.getValue() },
      m_moffat_index_updater {moffat_index, m_moffat_index},
      m_minkowski_distance_param { minkowski_distance_param.getValue() },
      m_minkowski_distance_param_updater {minkowski_distance_param, m_minkowski_distance_param},
      m_flat_top_offset { flat_top_offset.getValue() },
      m_flat_top_offset_updater {flat_top_offset, m_flat_top_offset}
{
}

double FlattenedMoffatComponent::getValue(double x, double y) {

  auto z = pow(pow(fabs(x), m_minkowski_distance_param) + pow(fabs(y), m_minkowski_distance_param),
      1 / m_minkowski_distance_param) -  m_flat_top_offset;

  if (z < 0) {
    return m_max_intensity;
  } else {
    return m_max_intensity * pow(1+z*z, -m_moffat_index);
  }
}

void FlattenedMoffatComponent::updateRasterizationInfo(double scale, double r_max) {
}

std::vector<ModelComponent::ModelSample> FlattenedMoffatComponent::getSharpSampling() {
  return {};
}

bool FlattenedMoffatComponent::insideSharpRegion(double x, double y) {
  return false;
}

}

