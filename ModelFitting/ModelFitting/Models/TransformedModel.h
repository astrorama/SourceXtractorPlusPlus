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
                BasicParameter& x, BasicParameter& y, std::tuple<double, double, double, double> transform);

  TransformedModel(std::vector<std::unique_ptr<ModelComponent>>&& component_list,
                BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation_angle, double width, double height,
                BasicParameter& x, BasicParameter& y);

  TransformedModel(TransformedModel&&) = default;

  virtual ~TransformedModel() = default;
};

}

#endif /* _MODELFITTING_MODELS_TRANSFORMEDMODEL_H_ */
