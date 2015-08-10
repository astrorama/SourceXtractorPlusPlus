/**
 * @file EngineParameter.h
 *
 * Created on: July 28, 2015
 *     Author: Pierre Dubath
 */

#ifndef ENGINE_PARAMETER_H_
#define ENGINE_PARAMETER_H_

#include <string>
#include <vector>
#include <memory>
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/CoordinateConverter.h"

namespace ModelFitting {

/**
 * @class EngineParameter
 * @brief
 * 		Simplest implementation of a parameter class
 *
 * @details
 * 		Simplest implementation of the abstract base class representing any parameter
 *
 */
class EngineParameter: public BasicParameter {

public:

  EngineParameter(const double world_value, std::unique_ptr<CoordinateConverter> converter) :
      BasicParameter(world_value), m_converter( move(converter) ) {
    m_engine_value = m_converter->worldToEngine(world_value);
  }

  double getEngineValue() const {
    return m_engine_value;
  }

  void updateEngineValue(double engine_value)
      {
        m_engine_value = engine_value;
        this->updateValueAndNotifyWatcher(m_converter->engineToWorld(engine_value));
      }

  /**
   * @brief
   *    The pure virtual setValue method
   * @details
   *    This method must be implemented in all implementation of this base class
   * @param value
   *    The value of the parameter
   *
   */
//  void setValue(const double value) {
//    m_value = value;
//  }
private:

  /// The parameter value in Engine coordinates
  double m_engine_value { 0.0 };

  std::unique_ptr<CoordinateConverter> m_converter {};

};

} // namespace ModelFitting

#endif /* ENGINE_PARAMETER_H_ */
