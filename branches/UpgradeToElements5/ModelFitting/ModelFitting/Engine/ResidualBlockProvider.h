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

