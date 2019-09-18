/*
 * NumericalDerivative.h
 *
 *  Created on: Sep 17, 2019
 *      Author: mschefer
 */

#ifndef _SEUTILS_NUMERICALDERIVATIVE_H_
#define _SEUTILS_NUMERICALDERIVATIVE_H_

#include <functional>

namespace SExtractor {

class NumericalDerivative {
public:

  static double centralDifference(std::function<double(double)> f, double x) {
    double h = 1e-6;

    // we need to consider that the rounding of those values could generate bad accuracy
    // so rather than using 2. * h, we have to use the actual difference
    // marked "volatile" to avoid possible compiler optimizations ruining this scheme
    volatile double x_plus = x + h;
    volatile double x_minus = x - h;
    double h_2 = x_plus - x_minus;

    return (f(x_plus) - f(x_minus)) / h_2;
  }

};


} // end SExtractor

#endif /* _SEUTILS_NUMERICALDERIVATIVE_H_ */
