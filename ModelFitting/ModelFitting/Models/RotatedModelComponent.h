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
 * @file RotatedModelComponent.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_ROTATEDMODELCOMPONENT_H
#define	MODELFITTING_ROTATEDMODELCOMPONENT_H

#include <memory> // for std::unique_ptr
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

/**
 * Decorates a model component, rotating the coordinates by the given angle.
 * As there is no scaling, the integrated value remains.
 */
class RotatedModelComponent : public ModelComponent {
  
public:

  /**
   * Constructor
   * @param component
   *    The component to be decorated. The ownership is acquired by the new object.
   * @param rotation_angle
   *    The rotation angle in radians.
   * @note
   *    Rotation is done anti-clockwise!
   */
  RotatedModelComponent(std::unique_ptr<ModelComponent> component,
      std::shared_ptr<BasicParameter> rotation_angle);

  RotatedModelComponent(RotatedModelComponent&& other);
            
  virtual ~RotatedModelComponent();
          
  double getValue(double x, double y) override;
  
  void updateRasterizationInfo(double scale, double r_max) override;
  
  std::vector<ModelSample> getSharpSampling() override;
  
  bool insideSharpRegion(double x, double y) override;
  
private:
  
  std::unique_ptr<ModelComponent> m_component;
  std::shared_ptr<BasicParameter> m_rotation_angle;

  //FIXME!!!!!!!!!!!!!! auto update those
  double m_cos;
  double m_sin;
  
}; // end of class RotatedModelComponent

} // end of namespace ModelFitting

#endif	/* MODELFITTING_ROTATEDMODELCOMPONENT_H */

