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
 * @file EngineParameter.h
 *
 * Created on: July 28, 2015
 *     Author: Pierre Dubath
 */

#ifndef ENGINE_PARAMETER_H_
#define ENGINE_PARAMETER_H_

#include <memory>

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/CoordinateConverter.h"

namespace ModelFitting {

/**
 * @class EngineParameter
 *
 * @brief EngineParameter are those derived from the minimization process
 *
 * @details
 *    EngineParameter is an extension of the BasicParameter class, involving
 *    coordinate transformation. It provides parameters with unbounded range
 *    which can be better handled in the minimization process. The "engine"
 *    terminology is in opposition with the "world" one, referring to parameter
 *    in physical units. Different possible conversions are implemented through
 *    the CoordinateConverter abstract base class.
 */
class EngineParameter: public BasicParameter {

public:

  EngineParameter(const double world_value, std::unique_ptr<CoordinateConverter> converter)
              : BasicParameter(world_value),
                m_engine_value{converter->worldToEngine(world_value)},
                m_converter{std::move(converter)} { }

  double getEngineValue() const {
    return m_engine_value;
  }

  void setEngineValue(const double engine_value);

  double getEngineToWorldDerivative() const;

  void setValue(const double value) override;

private:

  /// The parameter value in Engine coordinates
  double m_engine_value;

  /// The parameter converter
  std::unique_ptr<CoordinateConverter> m_converter;

};

} // namespace ModelFitting

#endif /* ENGINE_PARAMETER_H_ */
