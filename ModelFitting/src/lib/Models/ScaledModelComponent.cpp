/** 
 * @file ScaledModelComponent.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm> // for std::min
#include "ModelFitting/Models/ScaledModelComponent.h"

namespace ModelFitting {

ScaledModelComponent::ScaledModelComponent(std::unique_ptr<ModelComponent> component,
                                           BasicParameter& x_scale, BasicParameter& y_scale)
        : m_component {std::move(component)},
          m_x_scale{x_scale.getValue()}, m_y_scale{y_scale.getValue()},
          m_x_scale_updater{x_scale, m_x_scale}, m_y_scale_updater{y_scale, m_y_scale} {
}

ScaledModelComponent::ScaledModelComponent(ScaledModelComponent&& other)
          : m_component {std::move(other.m_component)},
            m_x_scale{other.m_x_scale}, m_y_scale{other.m_y_scale},
            m_x_scale_updater{other.m_x_scale_updater.getParameter(), m_x_scale},
            m_y_scale_updater{other.m_y_scale_updater.getParameter(), m_y_scale} {
}

ScaledModelComponent::~ScaledModelComponent() = default;

double ScaledModelComponent::getValue(double x, double y) {
  return m_component->getValue(x / m_x_scale, y / m_y_scale);
}

} // end of namespace ModelFitting