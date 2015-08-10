/**
 * @file BasicParameter.h
 *
 * Created on: July 26, 2015
 *     Author: Pierre Dubath
 */

#ifndef BASIC_PARAMETER_H_
#define BASIC_PARAMETER_H_

#include<vector>
#include<memory>

#include "ModelFitting/Parameters/ParameterWatcher.h"

namespace ModelFitting {

/**
 * @class BasicParameter
 * @brief
 * 		The parameter base class
 *
 * @details
 * 		The class representing a base class that any parameter class
 *    must implement (and extend)
 */
class BasicParameter {

public:
  /*
   * @brief Getter to access the private parameter value
   */
  double getValue() {
    return m_value;
  }

  /**
   * @brief
   *    The addWatcher method
   * @details
   *    This method ...
   *
   * @param watcher_ptr
   *    A watcher unique_ptr. Please remember that when using this method the pointer
   *    has to be moved to transfer its ownership to the method. The call is something like
   *
   *    addWatcher(move(a_watcher_ptr));
   *
   */
  void addWatcher(std::unique_ptr<ParameterWatcher> watcher_ptr);

  /**
   * @brief Destructor
   */
  virtual ~BasicParameter() {
  }

protected:

  //BasicParameter() {};

  BasicParameter(const double value) :
      m_value(value) {
  }

  /// Required implementation of the move constructor, which move the vector unique_ptr elements
  BasicParameter(BasicParameter&& bp) = default;
//  {
//    m_value = std::move(bp.m_value);
//    m_watcher_ptr_vector.resize(bp.m_watcher_ptr_vector.size());
//    std::move(bp.m_watcher_ptr_vector.begin(), bp.m_watcher_ptr_vector.end(),
//        m_watcher_ptr_vector.begin());
//  }

  BasicParameter& operator=(BasicParameter&& bp) = default;

  /**
   * @brief
   *    The updateValueAndNotifyWatcher method
   * @details
   *    This method update the parameter value and notify all watchers,
   *    by passing the new value calling their updatedValue() method.
   *    This method should never be overwritten
   * @param new_value
   *    The new value of the parameter
   *
   */
  void updateValueAndNotifyWatcher(const double new_value);

private:

  /// Parameter value
  double m_value { 0.0 };

  /// List of the parameter watcher
  std::vector<std::unique_ptr<ParameterWatcher>> m_watcher_ptr_vector;

};

} // namespace ModelFitting

#endif /* BASIC_PARAMETER_H_ */
