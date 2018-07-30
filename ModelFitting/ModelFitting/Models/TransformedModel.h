/*
 * TransformedModel.h
 *
 *  Created on: Jul 6, 2018
 *      Author: mschefer
 */

#ifndef _MODELFITTING_MODELS_TRANSFORMEDMODEL_H_
#define _MODELFITTING_MODELS_TRANSFORMEDMODEL_H_


#include "ModelFitting/Models/ExtendedModel.h"

namespace ModelFitting {

class TransformedModel : public ExtendedModel {

public:

  TransformedModel(std::vector<std::unique_ptr<ModelComponent>>&& component_list,
                BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation_angle, double width, double height,
                BasicParameter& x, BasicParameter& y, double jacobian[]);

//  TransformedModel(ExtendedModel&&);

  virtual ~TransformedModel() = default;

};

}

#endif /* _MODELFITTING_MODELS_TRANSFORMEDMODEL_H_ */
