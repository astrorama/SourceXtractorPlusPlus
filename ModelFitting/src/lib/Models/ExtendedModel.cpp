/** 
 * @file ExtendedModel.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include <numeric> // for std::accumulate
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/utils.h"

namespace ModelFitting {

ExtendedModel::ExtendedModel(std::vector<std::unique_ptr<ModelComponent> >&& component_list,
                             BasicParameter& x_scale, BasicParameter& y_scale,
                             BasicParameter& rotation_angle, double width, double height,
                             BasicParameter& x, BasicParameter& y)
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
