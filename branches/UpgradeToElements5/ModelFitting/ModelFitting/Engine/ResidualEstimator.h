/** 
 * @file ResidualEstimator.h
 * @date August 14, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_RESIDUALESTIMATOR_H
#define	MODELFITTING_RESIDUALESTIMATOR_H

#include <vector>
#include <memory>
#include <algorithm>
#include "ModelFitting/Engine/ResidualBlockProvider.h"

namespace ModelFitting {

/**
 * @class ResidualEstimator
 * 
 * @brief
 * Provides to the LeastSquareEngine the residual values
 * 
 * @details
 * This class does not compute the residuals itself. It operates as a collection
 * of objects implementing the ResidualBlockProvider interface, each one of
 * which is responsible for a block of residuals.
 * 
 * The proper way of using this class is to create an instance, register
 * ResidualBlockProvider instances by using the registerBlockProvider() method
 * and pass it to a LeastSquareEngine for solving the minimization problem. The
 * rest of the methods are dedicated for usage from inside the minimization
 * engine, so they can be safely be ignored.
 */
class ResidualEstimator {
  
public:
  
  /// Destructor
  virtual ~ResidualEstimator();
  
  /// Registers a ResidualBlockProvider to the ResidualEstimator
  void registerBlockProvider(std::unique_ptr<ResidualBlockProvider> provider);
  
  /// Returns the total number of residuals. It is equal with the sum of the
  /// sizes of all the registered block providers.
  std::size_t numberOfResiduals() const;
  
  /// Populates the elements where the given iterator points to with the
  /// current residual values. Note that the method makes the assumption that
  /// the iterator points to a structure which sufficient number of elements to
  /// fit the residuals. Failure of this precondition can lead to undefined
  /// behavior.
  template <typename DoubleIter>
  void populateResiduals(DoubleIter output_iter) const;
  
  /// Specialization of the populateResiduals() method for double*, which avoids
  /// alocating intermediate memory for the residuals.
  void populateResiduals(double* output_iter) const;
  
  /// Specialization of the populateResiduals() method for vector of doubles,
  /// which avoids alocating intermediate memory for the residuals.
  void populateResiduals(std::vector<double>::iterator output_iter) const;
  
private:
  
  std::size_t m_residual_no {0};
  std::vector<std::unique_ptr<ResidualBlockProvider>> m_block_provider_list {};
  
};

} // end of namespace ModelFitting

#include "_impl/ResidualEstimator.icpp"

#endif	/* MODELFITTING_RESIDUALESTIMATOR_H */

