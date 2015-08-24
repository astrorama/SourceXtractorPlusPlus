/** 
 * @file WorldValueResidual.h
 * @date August 14, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_WORLDVALUERESIDUAL_H
#define	MODELFITTING_WORLDVALUERESIDUAL_H

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Engine/ResidualBlockProvider.h"

namespace ModelFitting {

/**
 * @class WorldValueResidual
 * 
 * @brief
 * ResidualBlockProvider implementation for adding a prior to a parameter world
 * value
 * 
 * @details
 * The residual is calculated as:
 * \f[ r = w*(P_c-P_p) \f]
 * where:
 * - \f$w\f$: is the weight of the residual
 * - \f$P_c\f$: is the current world value of the parameter
 * - \f$P_p\f$: is the prior expected world value of the parameter
 */
class WorldValueResidual : public ResidualBlockProvider {
  
public:
  
  /**
   * @brief Constructs a new instance of WorldValueResidual
   * 
   * @details
   * Note that this class will add an observer to the parameter which requires
   * that the WorldValueResidual instance is still alive when it is called. For
   * this reason, any updates of a parameter value after a related WorldValueResidual
   * is deleted, leeds to undefined behavior.
   * 
   * @param parameter
   *    A reference to the parameter the residual is calculated for
   * @param expected_value
   *    The prior expected world value
   * @param weight
   *    The weight or the residual
   */
  WorldValueResidual(BasicParameter& parameter, double expected_value, double weight=1.);
  
  /// Destructor
  virtual ~WorldValueResidual();
  
  /// Always returns 1, as this class creates a single residual
  std::size_t numberOfResiduals() const override;
  
  /// Updates the value where the iterator points with the value of the residual
  /// for the current value of the parameter
  void populateResidualBlock(IterType output_iter) override;
  
private:
  
  double m_residual;
  
}; // end of class WorldValueResidual

} // end of namespace ModelFitting

#endif	/* MODELFITTING_WORLDVALUERESIDUAL_H */

