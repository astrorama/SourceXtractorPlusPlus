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
 * @file EngineParameter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */

#include "ModelFitting/Parameters/EngineParameter.h"

namespace ModelFitting {

using namespace std;

void EngineParameter::setEngineValue(const double engine_value) {
  m_engine_value = engine_value;
  BasicParameter::setValue(m_converter->engineToWorld(engine_value));
}

double EngineParameter::getEngineToWorldDerivative() const {
  return m_converter->getEngineToWorldDerivative(getValue());
}

void EngineParameter::setValue(const double value) {
  BasicParameter::setValue(value);
  m_engine_value = m_converter->worldToEngine(value);
}



} // namespace ModelFitting
