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
 * @file GSLEngine.h
 * @date April, 2019
 * @author Alejandro Alvarez Ayllon
 */

#ifndef MODELFITTING_GSLENGINE_H
#define	MODELFITTING_GSLENGINE_H

#include <gsl/gsl_machine.h>
#include "ModelFitting/Engine/LeastSquareEngine.h"

namespace ModelFitting {

/**
 * @class GSLEngine
 *
 * @brief
 *  LeastSquareEngine implementation which uses the GNU Scientific Library
 * @see
 *  https://www.gnu.org/software/gsl/doc/html/nls.html
 */
class GSLEngine : public LeastSquareEngine {

public:

  /**
   * @brief Constructs a new instance of the engine
   * @param itmax
   *    Maximum number of iteratios
   * @param xtol
   *    Step tolerance: \f|\delta_i| \le xtol(|x_i| + xtol)\f
   * @param gtol
   *    Gradient tolerance: \f\underset{i}{max}|g_i \times max(x_i, 1)| \le gtol \times max(\Phi(x),1)\f
   * @param ftol
   *    Tolerance for the change in \f\chi^2\f
   *    Some versions of the GSL library ignore this.
   * @param delta
   *    Step size for finite difference Jacobian
   */
  GSLEngine(int itmax = 1000, double xtol=1e-8, double gtol=1e-8, double ftol=1e-8, double delta=1e-4);

  /// Destructor
  virtual ~GSLEngine() = default;

  /// Solves the minimization problem using the levmar library. The returned
  /// summary contains as underlying_framework_info an std::array<double,10>,
  /// which keeps the information regarding the minimization as it is provided
  /// by levmar (for more info see http://users.ics.forth.gr/~lourakis/levmar).
  LeastSquareSummary solveProblem(EngineParameterManager& parameter_manager,
                                  ResidualEstimator& residual_estimator) override;

private:
  int m_itmax;
  double m_xtol, m_gtol, m_ftol, m_delta;
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_LEVMARENGINE_H */
