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
 * @file PositionedModel.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_POSITIONEDMODEL_H
#define	MODELFITTING_POSITIONEDMODEL_H

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"

namespace ModelFitting {

class PositionedModel {
  
public:
  
  PositionedModel(std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y);
          
  PositionedModel(PositionedModel&& other);

  virtual ~PositionedModel() = default;
  
  double getX() const;
  
  double getY() const;
  
private:
  std::shared_ptr<BasicParameter> m_x;
  std::shared_ptr<BasicParameter> m_y;

}; // end of class ConstantModel

} // end of namespace ModelFitting

#endif	/* MODELFITTING_POSITIONEDMODEL_H */

