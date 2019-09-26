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
 * @file NeutralConverter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */
#include "ModelFitting/Parameters/NeutralConverter.h"

namespace ModelFitting {

using namespace std;

NeutralConverter::~NeutralConverter() = default;

double NeutralConverter::worldToEngine(const double world_value) const {
  return world_value;
}

double NeutralConverter::engineToWorld(const double engine_value) const {
  return engine_value;
}

double NeutralConverter::getEngineToWorldDerivative(const double /*value*/) const {
  return 1;
}


}// namespace ModelFitting
