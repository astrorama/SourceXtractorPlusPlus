/**
 * @file CoordinateConverter.h
 *
 * Created on: July 28, 2015
 *     Author: Pierre Dubath
 */

#ifndef COORDINATE_CONVERTER_H_
#define COORDINATE_CONVERTER_H_

#include<string>
#include<vector>
#include<memory>

namespace ModelFitting {

/**
 * @class CoordinateConverter
 * @brief
 *    The coordinate converter interface
 *
 * @details
 *     Coordinate converter to transform parameter units used when computing model equation
 *     (i.e., world units) into units used in the minimization engine (i..e, engine units).
 *
 *     The conversion is necessary, because the minimization engine coordinate system must
 *     be unbounded (value from -inf to +inf) while some of the parameter world coordinate
 *     values can be bounded.
 *
 */
class CoordinateConverter {

public:

  /**
   * @brief
   *    World to engine coordinate converter
   *
   * @param world_value
   *    The value of the parameter in world coordinate
   *
   * @return engine_value
   *    The value of the parameter in engine coordinate
   *
   */
  virtual double worldToEngine(const double world_value) const = 0;

  /**
   * @brief
   *    Engine to world coordinate converter
   *
   * @param engine_value
   *    The value of the parameter in engine coordinate
   *
   */
  virtual double engineToWorld(const double engine_value) const = 0;

  /**
   * @brief Destructor
   */
  virtual ~CoordinateConverter() {
  }

};

} // namespace ModelFitting

#endif /* COORDINATE_CONVERTER_H_ */
