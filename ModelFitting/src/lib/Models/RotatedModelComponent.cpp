/** 
 * @file RotatedModelComponent.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include <cmath> // for std::cos, std::sin
#include "ModelFitting/Models/RotatedModelComponent.h"

namespace ModelFitting {

RotatedModelComponent::RotatedModelComponent(std::unique_ptr<ModelComponent> component,
                                             BasicParameter& rotation_angle)
        : m_component {std::move(component)},
          m_rotation_angle{rotation_angle.getValue()},
          m_cos{std::cos(m_rotation_angle)},
          m_sin{std::sin(m_rotation_angle)},
          m_rotation_angle_updater{rotation_angle, m_rotation_angle,
                                   ReferenceUpdater::PreAction{},
                                   [this](double) {
                                     m_cos = std::cos(m_rotation_angle);
                                     m_sin = std::sin(m_rotation_angle);
                                   }} {
}

RotatedModelComponent::RotatedModelComponent(RotatedModelComponent&& other)
        : m_component {std::move(other.m_component)},
          m_rotation_angle{other.m_rotation_angle},
          m_cos{other.m_cos}, m_sin{other.m_sin},
          m_rotation_angle_updater{other.m_rotation_angle_updater.getParameter(),
                                   m_rotation_angle, ReferenceUpdater::PreAction{},
                                   [this](double) {
                                     m_cos = std::cos(m_rotation_angle);
                                     m_sin = std::sin(m_rotation_angle);
                                   }} {
}

RotatedModelComponent::~RotatedModelComponent() = default;

double RotatedModelComponent::getValue(double x, double y) {
  double new_x = x * m_cos - y * m_sin;
  double new_y = x * m_sin + y * m_cos;
  return m_component->getValue(new_x, new_y);
}

void RotatedModelComponent::updateRasterizationInfo(double scale, double r_max) {
  m_component->updateRasterizationInfo(scale, r_max);
}

auto RotatedModelComponent::getSharpSampling() -> std::vector<ModelSample> {
  std::vector<ModelSample> result = m_component->getSharpSampling();
  for (auto& sample : result) {
    double new_x = std::get<0>(sample) * m_cos + std::get<1>(sample) * m_sin;
    double new_y = std::get<1>(sample) * m_cos - std::get<0>(sample) * m_sin;
    std::get<0>(sample) = new_x;
    std::get<1>(sample) = new_y;
  }
  return result;
}

bool RotatedModelComponent::insideSharpRegion(double x, double y) {
  double new_x = x * m_cos - y* m_sin;
  double new_y = x * m_sin + y * m_cos;
  return m_component->insideSharpRegion(new_x, new_y);
}

} // end of namespace ModelFitting
