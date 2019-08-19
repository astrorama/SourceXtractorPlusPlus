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
