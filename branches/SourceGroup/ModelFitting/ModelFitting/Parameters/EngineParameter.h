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
                m_engine_value{new double{converter->worldToEngine(world_value)}},
                m_converter{std::move(converter)} { }

  double getEngineValue() const {
    return *m_engine_value;
  }

  void setEngineValue(const double engine_value);

protected:
  /*
   * The setValue should not be implemented in this class, nor in any
   * extension
   */
  void setValue(const double value) = delete;

private:

  /// The parameter value in Engine coordinates
  std::shared_ptr<double> m_engine_value;

  /// The parameter converter
  std::shared_ptr<CoordinateConverter> m_converter;

};

} // namespace ModelFitting

#endif /* ENGINE_PARAMETER_H_ */
