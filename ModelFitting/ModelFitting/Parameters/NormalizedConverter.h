/** 
 * @file NormalizedConverter.h
 * @date August 18, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_NORMALIZEDCONVERTER_H
#define	MODELFITTING_NORMALIZEDCONVERTER_H

#include "ModelFitting/Parameters/CoordinateConverter.h"

namespace ModelFitting {

/**
 * @class NormalizedConverter
 * 
 * @brief
 * CoordinateConverter implementation which performs normalization
 */
class NormalizedConverter : public CoordinateConverter {
  
public:
  
  /**
   * @brief
   * Constructs a new instance of NormalizedConverter
   * @param norm_value
   *    The world value which will be normalized to 1 in engine coordinates
   */
  NormalizedConverter(double norm_value);
  
  /// Destructor
  virtual ~NormalizedConverter();
  
  /**
   * @brief
   * Returns the engine value for the given world value
   * @details
   * The engine value is world_value / norm_value, where norm_value is the
   * normalization value given to the constructor of the NormalizedConverter.
   * 
   * @param world_value
   *    The world value of the parameter
   * @return 
   *    The engine value of the parameter
   */
  double worldToEngine(const double world_value) const override;
  
  /**
   * @brief
   * Returns the world value for the given engine value
   * @details
   * The world value is engine_value * norm_value, where norm_value is the
   * normalization value given to the constructor of the NormalizedConverter.
   * 
   * @param engine_value
   *    The engine value of the parameter
   * @return 
   *    The world value of the parameter
   */
  double engineToWorld(const double engine_value) const override;
  
  double getEngineToWorldDerivative(const double value) const override;


private:
  
  double m_norm_value;
  
}; // end of class NormalizedConverter

} // end of namespace ModelFitting

#endif	/* MODELFITTING_NORMALIZEDCONVERTER_H */

