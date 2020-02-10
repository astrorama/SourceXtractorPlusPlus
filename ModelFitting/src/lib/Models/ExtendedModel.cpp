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
 * @file ExtendedModel.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include <numeric> // for std::accumulate
#include "AlexandriaKernel/memory_tools.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/Models/ExtendedModel.h"

using Euclid::make_unique;

namespace ModelFitting {

ExtendedModel::ExtendedModel(std::vector<std::unique_ptr<ModelComponent>>&& component_list,
    std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale,
    std::shared_ptr<BasicParameter> rotation_angle, double width, double height,
    std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y)
        : PositionedModel(x, y), m_width{width}, m_height{height} {
  for (auto& component : component_list) {
    auto scaled = make_unique<ScaledModelComponent>(std::move(component), x_scale, y_scale);
    auto rotated = make_unique<RotatedModelComponent>(std::move(scaled), rotation_angle);
    m_component_list.emplace_back(std::move(rotated));
  }
}

ExtendedModel::ExtendedModel(ExtendedModel&&) = default;

ExtendedModel::~ExtendedModel() = default;

double ExtendedModel::getValue(double x, double y) const {
  x -= getX();
  y -= getY();
  return std::accumulate(m_component_list.begin(), m_component_list.end(), 0.,
                         [x, y](double a, const std::unique_ptr<ModelComponent>& b) {
                           return a + b->getValue(x, y);
                         });
}

double ExtendedModel::getWidth() const {
  return m_width;
}

double ExtendedModel::getHeight() const {
  return m_height;
}

} // end of namespace ModelFitting
