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
/** 
 * @file ScaledModelComponent.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm> // for std::min
#include "ModelFitting/Models/ScaledModelComponent.h"

namespace ModelFitting {

ScaledModelComponent::ScaledModelComponent(std::unique_ptr<ModelComponent> component,
    std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale)
        : m_component {std::move(component)},
          m_x_scale{x_scale}, m_y_scale{y_scale} {
}

ScaledModelComponent::ScaledModelComponent(ScaledModelComponent&& other)
    : m_component {std::move(other.m_component)},
      m_x_scale{other.m_x_scale}, m_y_scale{other.m_y_scale} {
}

ScaledModelComponent::~ScaledModelComponent() = default;

double ScaledModelComponent::getValue(double x, double y) {
  return m_component->getValue(x / m_x_scale->getValue(), y / m_y_scale->getValue());
}

void ScaledModelComponent::updateRasterizationInfo(double scale, double r_max) {
  double new_scale = scale / std::min(m_x_scale->getValue(), m_y_scale->getValue());
  double new_r_max = r_max / std::min(m_x_scale->getValue(), m_y_scale->getValue());
  m_component->updateRasterizationInfo(new_scale, new_r_max);
}

auto ScaledModelComponent::getSharpSampling() -> std::vector<ModelSample> {
  double area_correction = m_x_scale->getValue() * m_y_scale->getValue();
  std::vector<ModelSample> result = m_component->getSharpSampling();
  for (auto& sample : result) {
    std::get<0>(sample) *= m_x_scale->getValue();
    std::get<1>(sample) *= m_y_scale->getValue();
    std::get<2>(sample) *= area_correction;
  }
  return result;
}

bool ScaledModelComponent::insideSharpRegion(double x, double y) {
  return m_component->insideSharpRegion(x / m_x_scale->getValue(), y / m_y_scale->getValue());
}

} // end of namespace ModelFitting
