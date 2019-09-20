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
 * @file SigmoidConverter.h
 *
 * Created on: July 28, 2015
 *     Author: Pierre Dubath
 */

#ifndef SIGMOID_CONVERTER_H_
#define SIGMOID_CONVERTER_H_

#include "ElementsKernel/Exception.h"
#include "ModelFitting/Parameters/CoordinateConverter.h"

namespace ModelFitting {

/**
 * @class SigmoidConverter
 * @brief
 *    CoordinateConverter implementation using the sigmoid function
 *
 */
class SigmoidConverter: public CoordinateConverter {

public:

  SigmoidConverter(const double min_value, const double max_value) :
      m_min_value(min_value), m_max_value(max_value) {
    if (m_min_value > m_max_value) {
      throw Elements::Exception()
          << "SigmoidConverter: min_value larger than max_value!";
    }
  }

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
  virtual ~SigmoidConverter();

private:

  /// minimum model value in world coordinates
  const double m_min_value;

  /// maximum model value in world coordinates
  const double m_max_value;
};

} // namespace ModelFitting

#endif /* SIGMOID_CONVERTER_H_ */
