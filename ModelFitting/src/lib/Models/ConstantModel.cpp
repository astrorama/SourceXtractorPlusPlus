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
 * @file ConstantModel.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Models/ConstantModel.h"

namespace ModelFitting {

ConstantModel::ConstantModel(BasicParameter& value)
        : m_value {value.getValue()}, m_value_updater{value, m_value} {
}

ConstantModel::ConstantModel(ConstantModel&& other)
    : m_value {other.m_value}, m_value_updater{other.m_value_updater.getParameter(), m_value} {
}

ConstantModel::~ConstantModel() = default;
    
double ConstantModel::getValue() const {
  return m_value;
}

} // end of namespace ModelFitting