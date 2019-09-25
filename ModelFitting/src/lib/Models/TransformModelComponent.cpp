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
 * TransformModelComponent.cpp
 *
 *  Created on: Jun 22, 2018
 *      Author: mschefer
 */

#include <cmath>
#include <assert.h>
#include "ModelFitting/Models/TransformModelComponent.h"

namespace ModelFitting {

TransformModelComponent::TransformModelComponent(
    std::unique_ptr<ModelComponent> component, std::tuple<double, double, double, double> transform)
    : m_component(std::move(component)) {

  m_transform[0] = std::get<0>(transform);
  m_transform[1] = std::get<1>(transform);
  m_transform[2] = std::get<2>(transform);
  m_transform[3] = std::get<3>(transform);

  double inv_det = 1.  / (m_transform[0] * m_transform[3] - m_transform[2] * m_transform[1]);
  m_inv_transform[0] = m_transform[3] * inv_det; m_inv_transform[1] = -m_transform[1] * inv_det;
  m_inv_transform[2] = -m_transform[2] * inv_det; m_inv_transform[3] = m_transform[0] * inv_det;

  assert(m_transform[0] * m_inv_transform[0] + m_transform[1] * m_inv_transform[2] >= 1 - 1e-8);
  assert(m_transform[0] * m_inv_transform[1] + m_transform[1] * m_inv_transform[3] <= 1e-8);
  assert(m_transform[2] * m_inv_transform[0] + m_transform[3] * m_inv_transform[2] <= 1e-8);
  assert(m_transform[2] * m_inv_transform[1] + m_transform[3] * m_inv_transform[3] >= 1 - 1e-8);
}

TransformModelComponent::TransformModelComponent(TransformModelComponent&& other) {
  for (int i = 0; i < 4; i++) {
    m_transform[i] = other.m_transform[i];
    m_inv_transform[i] = other.m_inv_transform[i];
  }
}

TransformModelComponent::~TransformModelComponent() {
}

double TransformModelComponent::getValue(double x, double y) {
  auto area_correction = fabs(m_transform[0] * m_transform[3] - m_transform[1] * m_transform[2]);

  double new_x = x * m_inv_transform[0] + y * m_inv_transform[2];
  double new_y = x * m_inv_transform[1] + y * m_inv_transform[3];
  return m_component->getValue(new_x, new_y) / area_correction;
}

void TransformModelComponent::updateRasterizationInfo(double scale, double r_max) {
  double x_scale = std::sqrt(m_transform[0] * m_transform[0] + m_transform[1] * m_transform[1]);
  double y_scale = std::sqrt(m_transform[2] * m_transform[2] + m_transform[3] * m_transform[3]);
  double new_scale = scale / std::min(x_scale, y_scale);
  double new_r_max = r_max / std::min(x_scale, y_scale);
  m_component->updateRasterizationInfo(new_scale, new_r_max);
}

std::vector<TransformModelComponent::ModelSample> TransformModelComponent::getSharpSampling() {
  std::vector<ModelSample> result = m_component->getSharpSampling();
  for (auto& sample : result) {
    double new_x = std::get<0>(sample) * m_transform[0] + std::get<1>(sample) * m_transform[2];
    double new_y = std::get<0>(sample) * m_transform[1] + std::get<1>(sample) * m_transform[3];
    std::get<0>(sample) = new_x;
    std::get<1>(sample) = new_y;
  }
  return result;
}

bool TransformModelComponent::insideSharpRegion(double x, double y) {
  double new_x = x * m_inv_transform[0] + y * m_inv_transform[2];
  double new_y = x * m_inv_transform[1] + y * m_inv_transform[3];
  return m_component->insideSharpRegion(new_x, new_y);
}

}
