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
 * FlattenedMoffatComponent.cpp
 *
 *  Created on: 2019 M01 7
 *      Author: mschefer
 */

#include <math.h>

#include "ModelFitting/Models/FlattenedMoffatComponent.h"

namespace ModelFitting {

FlattenedMoffatComponent::FlattenedMoffatComponent(std::shared_ptr<BasicParameter>  max_intensity, std::shared_ptr<BasicParameter> moffat_index,
    std::shared_ptr<BasicParameter> minkowski_distance_param, std::shared_ptr<BasicParameter> flat_top_offset) :
      m_max_intensity { max_intensity },
      m_moffat_index { moffat_index },
      m_minkowski_distance_param { minkowski_distance_param },
      m_flat_top_offset { flat_top_offset }
{
}

double FlattenedMoffatComponent::getValue(double x, double y) {

  auto z = pow(pow(fabs(x), m_minkowski_distance_param->getValue()) + pow(fabs(y), m_minkowski_distance_param->getValue()),
      1 / m_minkowski_distance_param->getValue()) -  m_flat_top_offset->getValue();

  if (z < 0) {
    return m_max_intensity->getValue();
  } else {
    return m_max_intensity->getValue() * pow(1+z*z, -m_moffat_index->getValue());
  }
}

void FlattenedMoffatComponent::updateRasterizationInfo(double, double) {
}

std::vector<ModelComponent::ModelSample> FlattenedMoffatComponent::getSharpSampling() {
  return {};
}

bool FlattenedMoffatComponent::insideSharpRegion(double, double) {
  return false;
}

}

