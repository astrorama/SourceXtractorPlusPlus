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