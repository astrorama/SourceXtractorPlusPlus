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
 * @file LogChiSquareComparator.h
 * @date August 19, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_LOGCHISQUARECOMPARATOR_H
#define	MODELFITTING_LOGCHISQUARECOMPARATOR_H

#include <cmath>

namespace ModelFitting {

/**
 * @class LogChiSquareComparator
 *
 * @brief
 * Data vs model comparator which computes a modified \f$\chi^2\f$ residual
 *
 * @details
 * The \f$\chi^2\f$ residual is modified to be more immune against occasionally
 * large non-Gaussian deviations. It is computed as:
 * \f[
 * g_{(u)} = \begin{cases}
 *  & u_0 * log\left( 1 + \dfrac{u}{u_0} \right) \text{ if } u \geqslant 0 \\
 *  & -u_0 * log\left( 1 - \dfrac{u}{u_0} \right) \text{ if } u < 0
 * \end{cases}
 * \f]
 * where:
 * - \f$u_0\f$: is a constant (typical value \f$\approx 10\f$)
 * - \f$ u=w*(r-m) \f$
 * - \f$w\f$: is the weight of the residual
 * - \f$r\f$: is the real value of the data
 * - \f$m\f$: is the corresponding model value
 */
class LogChiSquareComparator {

public:

  /// Creates a new LogChiSquareComparator instance with the given \f$u_0\f$
  LogChiSquareComparator(double u0=10.) : m_u0{u0} { }

  /// Returns the modified \f$\chi^2\f$ residual
  double operator()(double real, double model, double weight) const {
    double val =  weight * (real - model) / m_u0;
    return val>0. ? m_u0 * std::log1p(val) : -1. * m_u0 * std::log1p(-val);
  }

private:

  double m_u0;

}; // end of class LogChiSquareComparator

} // end of namespace ModelFitting



#endif	/* MODELFITTING_LOGCHISQUARECOMPARATOR_H */

