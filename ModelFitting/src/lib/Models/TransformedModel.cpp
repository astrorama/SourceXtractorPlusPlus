/*
 * TransformedModel.cpp
 *
 *  Created on: Jul 6, 2018
 *      Author: mschefer
 */

#include "ModelFitting/utils.h"

#include "ModelFitting/Models/TransformModelComponent.h"
#include "ModelFitting/Models/TransformedModel.h"

namespace ModelFitting {

TransformedModel::TransformedModel(std::vector<std::unique_ptr<ModelComponent>>&& component_list,
              BasicParameter& x_scale, BasicParameter& y_scale,
              BasicParameter& rotation_angle, double width, double height,
              BasicParameter& x, BasicParameter& y, double jacobian[])
      : ExtendedModel(std::move(component_list), x_scale, y_scale, rotation_angle, width, height, x, y)
{
  for (int i=0; i<m_component_list.size(); i++) {
    m_component_list[i] = make_unique<TransformModelComponent>(std::move(m_component_list[i]), jacobian);
  }
}

}

