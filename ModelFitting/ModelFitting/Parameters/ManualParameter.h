/**
 * @file ManualParameter.h
 *
 * Created on: July 26, 2015
 *     Author: Pierre Dubath
 */

#ifndef MANUAL_PARAMETER_H_
#define MANUAL_PARAMETER_H_

#include <string>
#include <vector>
#include <memory>
#include "ModelFitting/Parameters/BasicParameter.h"

namespace ModelFitting {

/**
 * @class ManualParameter
 * @brief
 * 		Simplest implementation of a parameter class
 *
 * @details
 * 		Simplest implementation of the abstract base class representing any parameter
 *
 */
class ManualParameter: public BasicParameter {

public:

  //ManualParameter() {}

  ManualParameter(const double value) :
      BasicParameter(value) {
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
  /**
   * @brief Destructor
   */
  virtual ~ManualParameter() {
  }

};

} // namespace ModelFitting

#endif /* MANUAL_PARAMETER_H_ */
