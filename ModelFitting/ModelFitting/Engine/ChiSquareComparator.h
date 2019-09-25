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
 * @file ChiSquareComparator.h
 * @date August 19, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_CHISQUARECOMPARATOR_H
#define	MODELFITTING_CHISQUARECOMPARATOR_H

namespace ModelFitting {

/**
 * @class ChiSquareComparator
 * 
 * @brief
 * Data vs model comparator which computes the \f$\chi^2\f$ residual
 * 
 * @details
 * The residual is computed as:
 * \f[\chi = w*(r-m)\f]
 * where:
 * - \f$w\f$: is the weight of the residual
 * - \f$r\f$: is the real value of the data
 * - \f$m\f$: is the corresponding model value
 */
class ChiSquareComparator {
  
public:
  
  /// Returns the \f$\chi^2\f$ residual
  double operator()(double real, double model, double weight) const {
    return weight * (real - model);
  }
  
}; // end of class ChiSquareComparator

} // end of namespace ModelFitting

#endif	/* MODELFITTING_CHISQUARECOMPARATOR_H */

