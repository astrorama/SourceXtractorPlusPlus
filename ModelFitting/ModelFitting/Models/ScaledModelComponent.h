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
/** 
 * @file ScaledModelComponent.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_SCALEDMODELCOMPONENT_H
#define	MODELFITTING_SCALEDMODELCOMPONENT_H

#include <memory> // for std::unique_ptr
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

/**
 * Decorates a model component, scaling the *axes* by the given parameters.
 * @details
 *  For instance, for a model scaled by 2 in each axes, a call to getValue(1,1) is
 *  equivalent to a getValue(0.5, 0.5) to the underlying function.
 *
 * @note
 *  The integral value is not kept, but rather increased approximately by (x_scale * y_scale)
 */
class ScaledModelComponent : public ModelComponent {
  
public:

  /**
   * Constructor
   * @param component
   *    The component to be decorated. The ownership is acquired by the new object.
   * @param x_scale
   *    Scale for the X axis
   * @param y_scale
   *    Scale for the Y axis
   */
  ScaledModelComponent(std::unique_ptr<ModelComponent> component,
                       std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale);

  ScaledModelComponent(ScaledModelComponent&& other);
            
  virtual ~ScaledModelComponent();
          
  double getValue(double x, double y) override;
  
  void updateRasterizationInfo(double scale, double r_max) override;
  
  std::vector<ModelSample> getSharpSampling() override;
  
  bool insideSharpRegion(double x, double y) override;
  
private:
  
  std::unique_ptr<ModelComponent> m_component;

  std::shared_ptr<BasicParameter> m_x_scale;
  std::shared_ptr<BasicParameter> m_y_scale;
}; // end of class ScaledModelComponent

} // end of namespace ModelFitting

#endif	/* MODELFITTING_SCALEDMODELCOMPONENT_H */

