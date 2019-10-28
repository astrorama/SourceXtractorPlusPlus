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
 * TransformModelComponent.h
 *
 *  Created on: Jun 20, 2018
 *      Author: mschefer
 */

#ifndef _MODELFITTING_MODELS_TRANSFORMMODELCOMPONENT_H_
#define _MODELFITTING_MODELS_TRANSFORMMODELCOMPONENT_H_

#include <memory> // for std::unique_ptr
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

/**
 * Decorates a model component, transforming the *axes* by the given transformation matrix
 *
 * @note
 *  Unlike ScaledModelComponent, this transformation keeps the integrated value
 */
class TransformModelComponent : public ModelComponent {

public:

  /**
   * Constructor
   * @param component
   *    The component to be decorated. The ownership is acquired by the new object.
   * @param transform
   *    The transformation matrix (column major order!)
   */
  TransformModelComponent(std::unique_ptr<ModelComponent> component, std::tuple<double, double, double, double> transform);

  TransformModelComponent(TransformModelComponent&& other);

  virtual ~TransformModelComponent();

  double getValue(double x, double y) override;

  void updateRasterizationInfo(double scale, double r_max) override;

  std::vector<ModelSample> getSharpSampling() override;

  bool insideSharpRegion(double x, double y) override;

private:

  std::unique_ptr<ModelComponent> m_component;

  double m_transform[4];
  double m_inv_transform[4];

}; // end of class TransformModelComponent

} // end of namespace ModelFitting


#endif /* _MODELFITTING_MODELS_TRANSFORMMODELCOMPONENT_H_ */
