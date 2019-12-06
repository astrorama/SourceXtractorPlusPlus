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
 * @file PointModel.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_POINTMODEL_H
#define	MODELFITTING_POINTMODEL_H

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Models/PositionedModel.h"
#include "ModelFitting/Models/ConstantModel.h"

namespace ModelFitting {

class PointModel : public PositionedModel, public ConstantModel {
  
public:
  
  PointModel( std::shared_ptr<BasicParameter> x,  std::shared_ptr<BasicParameter> y,  std::shared_ptr<BasicParameter> value);
  
  PointModel(PointModel&& other); 
  
  virtual ~PointModel();
  
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_POINTMODEL_H */

