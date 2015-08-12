/**
 * @file DependentParameter.h
 * @date August 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_DEPENDENTPARAMETER_H
#define MODELFITTING_DEPENDENTPARAMETER_H

#include <array>
#include "ModelFitting/Parameters/BasicParameter.h"

namespace ModelFitting {

/**
 * @class DependentParameter
 * @brief Implementation of a parameter depending on an arbitrary number
 *        of other parameters
 *
 * @details
 *    Variadic template implementation of a parameter depending on an arbitrary number
 *    of other parameters. The dependent parameter class does not store the input
 *    parameter themselves, but only there values in a private array. It also stores a
 *    function which allows to compute the dependent parameter value as a function of input
 *    parameter values. A dependent parameter cannot be copied, but only moved.
 *
 *    DepoendentParameter creation should be achieved using the factory method
 *    createDependentParameter(...) provide after the end of this class.
 *
 */
template<typename ... Parameters>
class DependentParameter: public BasicParameter {

public:

  static constexpr int PARAM_NO = sizeof...(Parameters);

  using ValueCalculator = std::function<double(decltype(std::declval<Parameters>().getValue())...)>;

  DependentParameter(ValueCalculator calculator, Parameters&... parameters)
  : BasicParameter {calculator(parameters.getValue()...)},
  m_calculator {std::move(calculator)},
  m_param_values {parameters.getValue()...} {
    inputParameterLoop(parameters...);
  }

  DependentParameter(const DependentParameter&) = delete;
  DependentParameter& operator=(const DependentParameter&) = delete;
  DependentParameter(DependentParameter&&) = default;
  DependentParameter& operator=(DependentParameter&&) = default;

  virtual ~DependentParameter() = default;

protected:

  void setValue(const double new_value) = delete;

private:

  /// function to calculate the dependent parameter value
  ValueCalculator m_calculator;
  /// Array of the input parameter values
  std::array<double, PARAM_NO> m_param_values;

  /* The two following methods represent the mecanism to loop over
   * the arbitrary number of variadic elements, the first one is called
   * as long as there is more than one elements in the list and the
   * second one is called when there is only one left.
   */
  template<typename First, typename ... Rest>
  void inputParameterLoop(First& first, Rest&... rest) {
    addParameterObserver(PARAM_NO - sizeof...(rest) - 1, first);
    inputParameterLoop(rest...);
  }

  template<typename Last>
  void inputParameterLoop(Last& last) {
    addParameterObserver(PARAM_NO - 1, last);
  }

  /* The two update methods below are called by the observer function
   * passed to the param.addObserver() method. They are used to transform
   * the array of input parameter values to a series of doubles
   * (val1, val2, ...) which is require to call the setValue (of the
   * BasicParameter class)
   */
  template <typename... ParamValues>
  void update(ParamValues... values) {
    update(values..., m_param_values[sizeof...(values)]);
  }

  void update(decltype(std::declval<Parameters>().getValue())... values) {
    /* Beware that it is the updated value (m_calculator(values...)) that
     * is passed to the setValue
     */
    BasicParameter::setValue(m_calculator(values...));
  }

  template<typename Param>
  void addParameterObserver(int i, Param& param) {
    param.addObserver([this, i](double v) {
          /// update the value of appropriate input parameter
          this->m_param_values[i] = v;
          /// called a method to update the dependent parameter value
          this->update(this->m_param_values[0]);
        });
  }

};

template<typename ... Parameters>
DependentParameter<Parameters...> createDependentParameter(
    typename DependentParameter<Parameters...>::ValueCalculator value_calculator,
    Parameters &... parameters) {
  return DependentParameter<Parameters...> { value_calculator, parameters... };
}

}

#endif  /* MODELFITTING_DEPENDENTPARAMETER_H */

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
