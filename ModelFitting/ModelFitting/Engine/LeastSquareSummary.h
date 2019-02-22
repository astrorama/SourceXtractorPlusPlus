/** 
 * @file LeastSquareSummary.h
 * @date August 21, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_LEASTSQUARESUMMARY_H
#define	MODELFITTING_LEASTSQUARESUMMARY_H

#include <vector>
#include <boost/any.hpp>

namespace ModelFitting {

/**
 * @class LeastSquareSummary
 * 
 * @brief
 * Class containing the summary information of solving a least square
 * minimization problem
 */
struct LeastSquareSummary {
  
  /// Flag indicating if the minimization was successful
  bool success_flag = true;
  
  /// The number of iterations
  size_t iteration_no {0};
  
  // 1-sigma margin of error for all the parameters
  std::vector<double> parameter_sigmas {};

  /// Info of the minimization process, as provided by the underlying framework.
  ///
  /// WARNING: Using this result will make your code compatible with only one
  /// specific implementation of LeastSquareEngine. This information is provided
  /// for completeness, but its usage is discouraged. For more information of
  /// the exact type refer to the documentation of the engine implementation
  /// you use.
  boost::any underlying_framework_info {};
  
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_LEASTSQUARESUMMARY_H */

