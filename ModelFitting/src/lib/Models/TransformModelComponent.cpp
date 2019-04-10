/*
 * TransformModelComponent.cpp
 *
 *  Created on: Jun 22, 2018
 *      Author: mschefer
 */

#include <cmath>
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
  m_inv_transform[0] = m_transform[0] * inv_det; m_inv_transform[1] = m_transform[2] * inv_det;
  m_inv_transform[2] = m_transform[1] * inv_det; m_inv_transform[3] = m_transform[3] * inv_det;
}

TransformModelComponent::TransformModelComponent(TransformModelComponent&& other) {
  for (int i=0; i<4; i++) {
    m_transform[i] = other.m_transform[i];
    m_inv_transform[i] = other.m_inv_transform[i];
  }
}

TransformModelComponent::~TransformModelComponent() {
}

double TransformModelComponent::getValue(double x, double y) {
  double new_x = x * m_inv_transform[0] + y * m_inv_transform[2];
  double new_y = x * m_inv_transform[1] + y * m_inv_transform[3];
  return m_component->getValue(new_x, new_y);
}

}
