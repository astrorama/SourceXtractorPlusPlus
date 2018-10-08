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

class Jacobian: public Property {
public:
  virtual ~Jacobian() = default;

  Jacobian(double a, double b, double c, double d): m_jacobian{a, b, c, d} {}

  const std::tuple<double, double, double, double>& asTuple() const {
    return m_jacobian;
  }

private:
  std::tuple<double, double, double, double> m_jacobian;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIAN_H_
