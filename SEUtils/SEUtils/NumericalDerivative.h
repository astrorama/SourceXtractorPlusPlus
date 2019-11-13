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
