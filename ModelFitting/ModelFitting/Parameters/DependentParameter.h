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
 * @file DependentParameter.h
 * @date August 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_DEPENDENTPARAMETER_H
#define MODELFITTING_DEPENDENTPARAMETER_H

#include <array>
#include <vector>
#include <memory>
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"

namespace ModelFitting {

/**
 * @class DependentParameter
 * @brief Implementation of a parameter depending on an arbitrary number
 *        of other parameters
 *
 * @details
 *    Variadic template implementation of a parameter depending on an arbitrary number
 *    of other parameters. A dependent parameter cannot be copied, but only moved.
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

  /**
   *  This constructore take a function which is used to compute the dependent
   *  parameter value as a function of input parameter values.
   */
  DependentParameter(ValueCalculator calculator, Parameters&... parameters)
  : BasicParameter {calculator(parameters.getValue()...)},
  m_calculator {new ValueCalculator{std::move(calculator)}},
  m_param_values {new std::array<double, PARAM_NO>{parameters.getValue()...}} {
    inputParameterLoop(parameters...);
    m_get_value_hook = std::bind(&DependentParameter::getValueHook, this);
  }

  virtual ~DependentParameter() = default;

protected:

  void setValue(const double new_value) = delete;
  void getValueHook(void) {
    if (!this->isObserved()) {
      this->update((*m_param_values)[0]);
    }
  }

  using BasicParameter::m_get_value_hook;

private:

  /// function to calculate the dependent parameter value
  std::shared_ptr<ValueCalculator> m_calculator;
  /*
   * Array of the input parameter values. The dependent parameter
   * class does not store the input parameter themselves, but only
   * there values in a private array.
   */
  std::shared_ptr<std::array<double, PARAM_NO>> m_param_values;
  
  std::shared_ptr<std::vector<std::unique_ptr<ReferenceUpdater>>> m_updaters {
    new std::vector<std::unique_ptr<ReferenceUpdater>>{}
  };

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
    update(values..., (*m_param_values)[sizeof...(values)]);
  }

  void update(decltype(std::declval<Parameters>().getValue())... values) {
    /* Beware that it is the updated value (m_calculator(values...)) that
     * is passed to the setValue
     */
    BasicParameter::setValue((*m_calculator)(values...));
  }

  template<typename Param>
  void addParameterObserver(int i, Param& param) {
    m_updaters->emplace_back(new ReferenceUpdater{
          param, (*m_param_values)[i],
          ReferenceUpdater::PreAction{},
          [this](double){
            // Do not bother updating live if there are no observers
            if (this->isObserved()) {
              this->update((*m_param_values)[0]);
            }
          }
    });
  }

};

template<typename ... Parameters>
DependentParameter<Parameters...> createDependentParameter(
    typename DependentParameter<Parameters...>::ValueCalculator value_calculator,
    Parameters &... parameters) {
  return DependentParameter<Parameters...> { value_calculator, parameters... };
}

template<typename ... Parameters>
std::shared_ptr<DependentParameter<Parameters...>> createDependentParameterPtr(
    typename DependentParameter<Parameters...>::ValueCalculator value_calculator,
    Parameters &... parameters) {
  return std::make_shared<DependentParameter<Parameters...>>(value_calculator, parameters...);
}

}

#endif  /* MODELFITTING_DEPENDENTPARAMETER_H */
