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
 * @file CoordinateConverter.h
 *
 * Created on: July 28, 2015
 *     Author: Pierre Dubath
 */

#ifndef COORDINATE_CONVERTER_H_
#define COORDINATE_CONVERTER_H_

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

  virtual double getEngineToWorldDerivative(const double value) const = 0;

  /**
   * @brief Destructor
   */
  virtual ~CoordinateConverter() {
  }

};

} // namespace ModelFitting

#endif /* COORDINATE_CONVERTER_H_ */
