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

  enum StatusFlag {
    SUCCESS, MAX_ITER, ERROR, MEMORY
  };

  /// Flag indicating if the minimization was successful
  StatusFlag status_flag = SUCCESS;
  
  /// The number of iterations
  size_t iteration_no {0};
  
  /// 1-sigma margin of error for all the parameters
  std::vector<double> parameter_sigmas {};

  /// Engine-specific reason for stopping the fitting
  int engine_stop_reason {0};

  /// Runtime (in seconds)
  float duration;

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

