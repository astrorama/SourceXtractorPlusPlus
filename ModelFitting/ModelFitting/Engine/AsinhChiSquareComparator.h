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
 * ASinhChiSquareComparator.h
 *
 *  Created on: 2019 M04 30
 *      Author: mschefer
 */

#ifndef _MODELFITTING_ENGINE_ASINHCHISQUARECOMPARATOR_H_
#define _MODELFITTING_ENGINE_ASINHCHISQUARECOMPARATOR_H_

#include <cstdlib>
#include <math.h>

namespace ModelFitting {

/**
 * @class AsinhChiSquareComparator
 *
 * @brief
 * Data vs model comparator which computes a modified \f$\chi^2\f$ residual, using asinh
 *
 * @details
 */
class AsinhChiSquareComparator {

public:

  AsinhChiSquareComparator(double u0=10) : m_u0{u0} { }

  /// Returns the modified \f$\chi^2\f$ residual
  double operator()(double real, double model, double weight) const {
    float val =  weight * (real - model) / m_u0;
    return m_u0 * std::asinh(val);
  }

private:

  double m_u0;

}; // end of class AsinhChiSquareComparator

} // end of namespace ModelFitting

#endif /* _MODELFITTING_ENGINE_ASINHCHISQUARECOMPARATOR_H_ */
