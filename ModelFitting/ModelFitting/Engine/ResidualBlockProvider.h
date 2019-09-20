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
 * @file ResidualBlockProvider.h
 * @date August 14, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_RESIDUALBLOCKPROVIDER_H
#define	MODELFITTING_RESIDUALBLOCKPROVIDER_H

namespace ModelFitting {

/**
 * @class ResidualBlockProvider
 * 
 * @brief
 * Interface of a class which can provide a block of residuals for least square
 * minimization solving
 */
class ResidualBlockProvider {
  
public:
  
  /** 
   * The type of the iterator the populateResiduals() method receives. Note that
   * even though this type is part of the public API, the only guarantee is that
   * is is an output iterator type. It is not a template parameter because of
   * the limitation of C++ having virtual template methods.
   */
  using IterType = double*;
  
  /**
   * @brief Returns the number of residuals provided by this provider
   * 
   * @details
   * Implementations must guarantee that this number will be constant for the
   * lifetime of an instance. Failure of this restriction might result to
   * undefined behavior.
   * 
   * @return The number of residuals
   */
  virtual std::size_t numberOfResiduals() const = 0;
  
  /**
   * @brief Provides the residual values
   * 
   * @details
   * Implementations of this method should use the given iterator for populating
   * their residual values. The iterator should not be increased more times than
   * the number of residuals reported by the numberOfResiduals() method. Such an
   * action might result to undefined behavior.
   * 
   * @param output_iter
   *    The iterator to use for returning the residual values
   */
  virtual void populateResidualBlock(IterType output_iter) = 0;
  
  /// Destructor
  virtual ~ResidualBlockProvider() = default;
  
}; // end of class ResidualBlockProvider

} // end of namespace ModelFitting

#endif	/* MODELFITTING_RESIDUALBLOCKPROVIDER_H */

