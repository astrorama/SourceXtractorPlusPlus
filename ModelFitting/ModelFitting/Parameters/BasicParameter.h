/**
 * @file BasicParameter.h
 * @date August 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_BASICPARAMETER_H
#define MODELFITTING_BASICPARAMETER_H

#include <functional>
#include <vector>

namespace ModelFitting {

/**
 * @class BasicParameter
 * @brief
 *    The parameter base class
 *
 * @details
 *    Base class that any parameter class must implement (and extend).
 *    It includes the parameter value and a list of observers, which
 *    are being notified of any value change. The observers can be any
 *    function which are being called with new value when it changes.
 */
class BasicParameter {

public:

  /*
   * Observers are functions
   */
  using ParameterObserver = std::function<void(double)>;

  virtual ~BasicParameter() = default;

  /*
   * @brief Getter to access the private parameter value
   */
  double getValue() {
    return m_value;
  }

  /**
   * @brief
   *    The addObserver method adding observer to the member
   *    list
   */
  void addObserver(ParameterObserver observer) {
    m_observer_list.emplace_back(std::move(observer));
  }

protected:

  BasicParameter(const double value) :
      m_value { value } {
  }

  BasicParameter(const BasicParameter&) = delete;
  BasicParameter& operator=(const BasicParameter&) = delete;
  BasicParameter(BasicParameter&&) = default;
  BasicParameter& operator=(BasicParameter&&) = default;

  /*
   * @brief Setter for the new value, which also notify the
   * observers, by calling them with the new value.
   */
  void setValue(const double new_value) {
    m_value = new_value;
    for (auto& observer : m_observer_list) {
      observer(m_value);
    }
  }

private:

  double m_value;
  std::vector<ParameterObserver> m_observer_list { };

};

}

#endif  /* MODELFITTING_BASICPARAMETER_H */

//**
// * @file BasicParameter.h
// *
// * Created on: July 26, 2015
// *     Author: Pierre Dubath
// */
//
//#ifndef BASIC_PARAMETER_H_
//#define BASIC_PARAMETER_H_
//
//#include<vector>
//#include<memory>
//
//#include "ModelFitting/Parameters/ParameterWatcher.h"
//
//namespace ModelFitting {
//
///**
// * @class BasicParameter
// * @brief
// * 		The parameter base class
// *
// * @details
// * 		The class representing a base class that any parameter class
// *    must implement (and extend)
// */
//class BasicParameter {
//
//public:
//  /*
//   * @brief Getter to access the private parameter value
//   */
//  double getValue() {
//    return m_value;
//  }
//
//  /**
//   * @brief
//   *    The addWatcher method
//   * @details
//   *    This method ...
//   *
//   * @param watcher_ptr
//   *    A watcher unique_ptr. Please remember that when using this method the pointer
//   *    has to be moved to transfer its ownership to the method. The call is something like
//   *
//   *    addWatcher(move(a_watcher_ptr));
//   *
//   */
//  void addWatcher(std::unique_ptr<ParameterWatcher> watcher_ptr);
//
//  /**
//   * @brief Destructor
//   */
//  virtual ~BasicParameter() {
//  }
//
//protected:
//
//  //BasicParameter() {};
//
//  BasicParameter(const double value) :
//      m_value(value) {
//  }
//
//  /// Required implementation of the move constructor, which move the vector unique_ptr elements
//  BasicParameter(BasicParameter&& bp) = default;
////  {
////    m_value = std::move(bp.m_value);
////    m_watcher_ptr_vector.resize(bp.m_watcher_ptr_vector.size());
////    std::move(bp.m_watcher_ptr_vector.begin(), bp.m_watcher_ptr_vector.end(),
////        m_watcher_ptr_vector.begin());
////  }
//
//  BasicParameter& operator=(BasicParameter&& bp) = default;
//
//  /**
//   * @brief
//   *    The updateValueAndNotifyWatcher method
//   * @details
//   *    This method update the parameter value and notify all watchers,
//   *    by passing the new value calling their updatedValue() method.
//   *    This method should never be overwritten
//   * @param new_value
//   *    The new value of the parameter
//   *
//   */
//  void updateValueAndNotifyWatcher(const double new_value);
//
//private:
//
//  /// Parameter value
//  double m_value;
//
//  /// List of the parameter watcher
//  std::vector<std::unique_ptr<ParameterWatcher>> m_watcher_ptr_vector {};
//
//};
//
//} // namespace ModelFitting
//
//#endif /* BASIC_PARAMETER_H_ */
