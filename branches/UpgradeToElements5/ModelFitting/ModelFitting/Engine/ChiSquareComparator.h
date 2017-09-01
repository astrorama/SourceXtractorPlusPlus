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

