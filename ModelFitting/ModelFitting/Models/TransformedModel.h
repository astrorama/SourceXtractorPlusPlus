/*
 * TransformedModel.h
 *
 *  Created on: Jul 6, 2018
 *      Author: mschefer
 */

#ifndef _MODELFITTING_MODELS_TRANSFORMEDMODEL_H_
#define _MODELFITTING_MODELS_TRANSFORMEDMODEL_H_


#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/TransformModelComponent.h"

namespace ModelFitting {

template <typename ImageType>
class TransformedModel : public ExtendedModel<ImageType> {

  using ExtendedModel<ImageType>::m_component_list;

public:

  TransformedModel(std::vector<std::unique_ptr<ModelComponent>>&& component_list,
                BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation_angle, double width, double height,
                BasicParameter& x, BasicParameter& y, std::tuple<double, double, double, double> transform)
        : ExtendedModel<ImageType>(std::move(component_list), x_scale, y_scale, rotation_angle, width, height, x, y)
  {
    for (unsigned int i=0; i<m_component_list.size(); i++) {
      m_component_list[i] = make_unique<TransformModelComponent>(std::move(m_component_list[i]), transform);
    }
  }

  TransformedModel(std::vector<std::unique_ptr<ModelComponent>>&& component_list,
                BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation_angle, double width, double height,
                BasicParameter& x, BasicParameter& y)
        : ExtendedModel<ImageType>(std::move(component_list), x_scale, y_scale, rotation_angle, width, height, x, y)
  {
    auto transform = std::make_tuple(1, 0, 0, 1);
    for (unsigned int i=0; i<m_component_list.size(); i++) {
      m_component_list[i] = make_unique<TransformModelComponent>(std::move(m_component_list[i]), transform);
    }
  }

  TransformedModel(TransformedModel&&) = default;

  virtual ~TransformedModel() = default;
};

}

#endif /* _MODELFITTING_MODELS_TRANSFORMEDMODEL_H_ */
