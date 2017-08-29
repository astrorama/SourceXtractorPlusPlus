/*
 * NormalizedPositiveParameter.h
 *
 *  Created on: Jun 29, 2017
 *      Author: mschefer
 */

#ifndef _MODELFITTING_PARAMETERS_NORMALIZEDPOSITIVECONVERTER_H_
#define _MODELFITTING_PARAMETERS_NORMALIZEDPOSITIVECONVERTER_H_

#include "ModelFitting/Parameters/CoordinateConverter.h"

namespace ModelFitting {

/**
 * @class NormalizedPositiveConverter
 *
 * @brief
 * CoordinateConverter implementation which performs normalization
 */
class NormalizedPositiveConverter : public CoordinateConverter {

public:

  /**
   * @brief
   * Constructs a new instance of NormalizedPositiveConverter
   * @param norm_value
   *    The world value which will be normalized to 1 in engine coordinates
   */
  NormalizedPositiveConverter(double norm_value) : m_norm_value{norm_value} { }

  /// Destructor
  virtual ~NormalizedPositiveConverter() = default;

  /**
   * @brief
   * Returns the engine value for the given world value
   * @details
   * The engine value is world_value / norm_value, where norm_value is the
   * normalization value given to the constructor of the NormalizedPositiveConverter.
   *
   * @param world_value
   *    The world value of the parameter
   * @return
   *    The engine value of the parameter
   */
  double worldToEngine(const double world_value) const override {
    return world_value / m_norm_value;
  }

  /**
   * @brief
   * Returns the world value for the given engine value
   * @details
   * The world value is engine_value * norm_value, where norm_value is the
   * normalization value given to the constructor of the NormalizedPositiveConverter.
   *
   * @param engine_value
   *    The engine value of the parameter
   * @return
   *    The world value of the parameter
   */
  double engineToWorld(const double engine_value) const override  {
    return fabs(engine_value * m_norm_value);
  }



private:

  double m_norm_value;

}; // end of class NormalizedPositiveConverter

} // end of namespace ModelFitting


#endif /* _MODELFITTING_PARAMETERS_NORMALIZEDPOSITIVECONVERTER_H_ */
