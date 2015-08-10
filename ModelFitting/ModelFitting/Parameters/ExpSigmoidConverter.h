/**
 * @file ExpExpSigmoidConverter.h
 *
 * Created on: July 29, 2015
 *     Author: Pierre Dubath
 */

#ifndef EXP_SIGMOID_CONVERTER_H_
#define EXP_SIGMOID_CONVERTER_H_

#include<string>
#include<memory>
#include<math.h>
#include "ModelFitting/Parameters/CoordinateConverter.h"

namespace ModelFitting {

/**
 * @class ExpSigmoidConverter
 * @brief
 *    CoordinateConverter implementation using the Exponantiated Sigmoid function
 *
 */
class ExpSigmoidConverter: public CoordinateConverter {

public:

  ExpSigmoidConverter() = delete;

  ExpSigmoidConverter(const double min_value, const double max_value) :
      m_min_value(min_value), m_max_value(max_value) {
    if (m_min_value > m_max_value) {
      throw Elements::Exception()
          << "ExpSigmoidConverter: min_value larger than max_value!";
    }
    if (m_min_value <= 0.0) {
      throw Elements::Exception() << "ExpSigmoidConverter: min_value is not a positive value!";
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
  double worldToEngine(const double world_value) const {
    if (world_value < m_min_value || world_value > m_max_value) {
      throw Elements::Exception()
          << "WorldToEngine ExpSigmoidConverter: world values outside of possible range";
    }
    return log(world_value / m_min_value) / log((m_max_value / world_value));
  }

  /**
   * @brief
   *    Engine to world coordinate converter
   *
   * @param engine_value
   *
   * @return world_value
   *
   */
  double engineToWorld(const double engine_value) const {
    return m_min_value * exp( log(m_max_value / m_min_value) / (1 + exp(-engine_value)) );
  }

  /**
   * @brief Destructor
   */
  virtual ~ExpSigmoidConverter() {
  }

private:

  /// minimum model value in world coordinates
  const double m_min_value { 10e-10 };

  /// maximum model value in world coordinates
  const double m_max_value { 0.0 };

};

} // namespace ModelFitting

#endif /* EXP_SIGMOID_CONVERTER_H_ */
