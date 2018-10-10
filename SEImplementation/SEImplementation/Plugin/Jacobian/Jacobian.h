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
