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
 * @file NormalizedConverter.cpp
 * @date August 18, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Parameters/NormalizedConverter.h"

namespace ModelFitting {

NormalizedConverter::NormalizedConverter(double norm_value) : m_norm_value{norm_value} { }

NormalizedConverter::~NormalizedConverter() = default;

double NormalizedConverter::worldToEngine(const double world_value) const {
  return world_value / m_norm_value;
}

double NormalizedConverter::engineToWorld(const double engine_value) const {
  return engine_value * m_norm_value;
}

double NormalizedConverter::getEngineToWorldDerivative(const double /*value*/) const {
  return m_norm_value;
}

} // end of namespace ModelFitting
