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
 * @file ConstantModel.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_CONSTANTMODEL_H
#define	MODELFITTING_CONSTANTMODEL_H

#include "ModelFitting/Parameters/BasicParameter.h"

namespace ModelFitting {

class ConstantModel {
  
public:
  
  ConstantModel( std::shared_ptr<BasicParameter> value);
          
  ConstantModel(ConstantModel&& other);

  virtual ~ConstantModel();
  
  double getValue() const;
  
private:
  std::shared_ptr<BasicParameter> m_value;

}; // end of class ConstantModel

} // end of namespace ModelFitting

#endif	/* MODELFITTING_CONSTANTMODEL_H */

