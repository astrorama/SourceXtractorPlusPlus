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

class BasicParameter {

public:

  using ParameterObserver = std::function<void(double)>;

  virtual ~BasicParameter() = default;

  double getValue() {
    return m_value;
  }

  void addObserver(ParameterObserver observer) {
    m_observer_list.emplace_back(std::move(observer));
  }

protected:

  BasicParameter(const double value) : m_value{value} { }

  BasicParameter(const BasicParameter&) = delete;
  BasicParameter& operator=(const BasicParameter&) = delete;
  BasicParameter(BasicParameter&&) = default;
  BasicParameter& operator=(BasicParameter&&) = default;

  void setValue(const double new_value) {
    m_value = new_value;
    for (auto& observer : m_observer_list) {
      observer(m_value);
    }
  }

private:

  double m_value;
  std::vector<ParameterObserver> m_observer_list {};

};

}

#endif  /* MODELFITTING_BASICPARAMETER_H */




///**
// * @file DependentParameter.h
// *
// * Created on: July 30, 2015
// *     Author: Pierre Dubath and Nikolaos Apostolakos
// */
//
//#ifndef DEPENDENT_PARAMETER_H_
//#define DEPENDENT_PARAMETER_H_
//
//#include <string>
//#include <iostream>
//#include <vector>
//#include <memory>
//#include <functional>
//
//#include "ModelFitting/Parameters/BasicParameter.h"
//#include "ModelFitting/Parameters/ParameterWatcher.h"
//
//namespace ModelFitting {
//
///**
// * @class DependentParameter
// * @brief
// * 		Simplest implementation of a parameter class
// *
// * @details
// * 		Simplest implementation of the abstract base class representing any parameter
// *
// */
//template<typename ... Parameters>
//class DependentParameter: public BasicParameter {
//
//public:
//
//  using ValueCalculator = std::function<double(const std::array<double, sizeof...(Parameters)>&)>;
//
//  DependentParameter(ValueCalculator value_calculator,
//      Parameters &... parameters) :
//      BasicParameter(value_calculator( { parameters.getValue() ... })) {
//    inputParameterLoop<sizeof...(Parameters), Parameters...>(m_parameter_values, parameters...);
//    //std::cout << m_parameter_values[0] << " " << m_parameter_values[1] << std::endl;
//  }
//
//  /// TODO What if this basic already has watcher? Should we copy the watchers
//  DependentParameter(DependentParameter<Parameters...>&& dp) : BasicParameter(std::move(dp.getValue())) {
//    m_value_calculator = std::move(dp.m_value_calculator);
//    m_parameter_values = std::move(dp.m_parameter_values);
//  }
//
//  /**
//   * @brief
//   *    The pure virtual setValue method
//   * @details
//   *    This method must be implemented in all implementation of this base class
//   * @param value
//   *    The value of the parameter
//   *
//   */
//  std::vector<double> getInputParameterValues() const {
//    std::vector<double> valueVector {};
//    for(auto parameter_values : m_parameter_values) {
//      valueVector.push_back(parameter_values);
//    }
//    return (std::move(valueVector));
//  }
//
//  /**
//   * @brief Destructor
//   */
//  virtual ~DependentParameter() {
//  }
//
//private:
//
//  template<int I, typename First, typename ... Rest>
//  void inputParameterLoop(std::array<double, I>& values, First& first,
//      Rest &... rest) {
//    /// index of the input parameter value
//    int index = I - sizeof...(Rest) - 1;
//    /// store the input parameter value into the dependent parameter value array
//    values[index] = first.getValue();
//    ///
//    std::unique_ptr<DependentParameterWatcher> dpw_ptr(new DependentParameterWatcher(values[index], *this));
//    first.addWatcher(std::move(dpw_ptr));
//    inputParameterLoop<I, Rest...>(values, rest...);
//  }
//
//  template<int I, typename Last>
//  void inputParameterLoop(std::array<double, I>& values, Last& last) {
//    /// index of the input parameter value
//    int index = I - 1;
//    values[index] = last.getValue();
//    std::unique_ptr<DependentParameterWatcher> dpw_ptr(new DependentParameterWatcher(values[index], *this));
//    last.addWatcher(std::move(dpw_ptr));
//    //DependentParameterWatcher dpw {values[index], *this};
//  }
//
//  /// The value calculator
//  ValueCalculator m_value_calculator;
//
//  //int m_num_input_parameters {sizeof...(Parameters)};
//
//  /// The values of the basics parameters
//  std::array<double, sizeof...(Parameters)> m_parameter_values {};
//
//  //WatcherForDependentParameter<DependentParameter<Parameters...>> test;
//
//  //std::array<WatcherForDependentParameter<DependentParameter<Parameters...>>, sizeof...(Parameters)> m_parameter_watchers {};
//
//  //std::array<unique_ptr<BasicParameter>, sizeof...(Parameters)> m_parameter_watchers {};
//
//  /**
//   * @class DependentParameterWatcher
//   * @brief
//   *    The parameter update notification
//   *
//   * @details
//   *
//   *
//   */
//  class DependentParameterWatcher: public ParameterWatcher {
//
//  public:
//    /**
//     * @brief Destructor
//     */
//    virtual ~DependentParameterWatcher() {
//    }
//
//
//    //DependentParameterWatcher() {}
//    DependentParameterWatcher(double& value, DependentParameter<Parameters...>& parent) :
//    m_value(value), m_parent(parent) {
//    }
//
//    /**
//     * @brief
//     *    The pure virtual updatedValue(double new_value) method
//     * @details
//     *    The implementation must define the operations that the
//     *    ParameterWather wants to carry out when the parameter
//     *    value is updated. and provided through this method.
//     *
//     * @param new_value
//     *    The updated value of the parameter
//     *
//     */
//    void updatedValue(double new_value) override {
//      //m_dependent_parameter.updatedValue(new_value, m_index);
//    }
//
//  private:
//
//
//    double& m_value;
//
//    DependentParameter<Parameters...>& m_parent;
//
//  };
//
//};
//
//template<typename ... Parameters>
//DependentParameter<Parameters...> createDependentParameter(
//    typename DependentParameter<Parameters...>::ValueCalculator value_calculator,
//    Parameters &... parameters) {
//  return std::move(DependentParameter<Parameters...> { value_calculator,
//      parameters... });
//}
//
//} // namespace ModelFitting
//
//#endif /* DEPENDENT_PARAMETER_H_ */
