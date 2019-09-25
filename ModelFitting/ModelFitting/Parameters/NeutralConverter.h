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
 * @file NeutralConverter.h
 *
 * Created on: July 28, 2015
 *     Author: Pierre Dubath
 */

#ifndef NEUTRAL_CONVERTER_H_
#define NEUTRAL_CONVERTER_H_

#include "ModelFitting/Parameters/CoordinateConverter.h"


namespace ModelFitting {

/**
 * @class NeutralConverter
 * @brief
 *    CoordinateConverter implementation which performs no conversion
 *
 */
class NeutralConverter : public CoordinateConverter{

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
  double worldToEngine(const double world_value) const override;

  /**
   * @brief
   *    Engine to world coordinate converter
   *
   * @param engine_value
   *
   * @return world_value
   *
   */
  double engineToWorld(const double engine_value) const override;

  double getEngineToWorldDerivative(const double value) const override;

  /**
   * @brief Destructor
   */
  virtual ~NeutralConverter();

};

} // namespace ModelFitting

#endif /* NEUTRAL_CONVERTER_H_ */
