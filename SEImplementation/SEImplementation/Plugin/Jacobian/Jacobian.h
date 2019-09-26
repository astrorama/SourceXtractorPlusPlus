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
/*
 * Jacobian.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */


#ifndef _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIAN_H_
#define _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIAN_H_

#include <tuple>
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class Jacobian {
public:
  virtual ~Jacobian() = default;

  // Identity constructor for convenience
  Jacobian(): m_jacobian{1., 0., 0., 1.} {}

  Jacobian(double a, double b, double c, double d): m_jacobian{a, b, c, d} {}

  const std::tuple<double, double, double, double>& asTuple() const {
    return m_jacobian;
  }

private:
  std::tuple<double, double, double, double> m_jacobian;
};


class JacobianSource: public Property, public Jacobian {
public:
  using Jacobian::Jacobian;
};

class JacobianGroup: public Property, public Jacobian {
  using Jacobian::Jacobian;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIAN_H_
