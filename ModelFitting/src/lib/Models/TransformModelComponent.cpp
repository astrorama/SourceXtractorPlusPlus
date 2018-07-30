/*
 * TransformModelComponent.cpp
 *
 *  Created on: Jun 22, 2018
 *      Author: mschefer
 */

#include "ModelFitting/Models/TransformModelComponent.h"

namespace ModelFitting {

TransformModelComponent::TransformModelComponent(
    std::unique_ptr<ModelComponent> component, double jacobian[])
    : m_component(std::move(component)) {
  for (int i=0; i<4; i++) {
    m_transform[i] = jacobian[i];
  }

  double inv_det = 1.  / (m_transform[0] * m_transform[3] - m_transform[2] * m_transform[1]);
  m_inv_transform[0] = jacobian[3] * inv_det; m_inv_transform[1] = jacobian[1] * inv_det;
  m_inv_transform[2] = jacobian[2] * inv_det; m_inv_transform[3] = jacobian[0] * inv_det;
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
  double new_x = x * m_transform[0] + y * m_transform[1];
  double new_y = x * m_transform[2] + y * m_transform[3];
  return m_component->getValue(new_x, new_y);
}

void TransformModelComponent::updateRasterizationInfo(double scale, double r_max) {
  //FIXME?
  m_component->updateRasterizationInfo(scale, r_max);
}

std::vector<TransformModelComponent::ModelSample> TransformModelComponent::getSharpSampling() {
  std::vector<ModelSample> result{};

  for (auto& sample : m_component->getSharpSampling()) {
    double new_x = std::get<0>(sample) * m_inv_transform[0] + std::get<1>(sample) * m_inv_transform[1];
    double new_y = std::get<1>(sample) * m_inv_transform[2] + std::get<0>(sample) * m_inv_transform[3];
    result.emplace_back(new_x, new_y, std::get<2>(sample));
  }
  return result;
}

bool TransformModelComponent::insideSharpRegion(double x, double y) {
  double new_x = x * m_transform[0] + y * m_transform[1];
  double new_y = x * m_transform[2] + y * m_transform[3];
  return m_component->insideSharpRegion(new_x, new_y);
}

}

