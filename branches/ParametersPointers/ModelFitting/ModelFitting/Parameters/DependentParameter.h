/**
 * @file DependentParameter.h
 * @date August 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_DEPENDENTPARAMETER_H
#define MODELFITTING_DEPENDENTPARAMETER_H

#include <array>
#include <memory>
#include <vector>
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
   *  This constructor take a function which is used to compute the dependent
   *  parameter value as a function of input parameter values.
   */
  DependentParameter(ValueCalculator calculator, std::shared_ptr<Parameters>... parameters)
  : BasicParameter {calculator(parameters->getValue()...)},
  m_calculator {std::move(calculator)},
  m_param_values {parameters->getValue()...} {
    inputParameterLoop(parameters...);
  }

  virtual ~DependentParameter() = default;

protected:

  void setValue(const double new_value) = delete;

private:

  /// function to calculate the dependent parameter value
  ValueCalculator m_calculator;
  /*
   * Array of the input parameter values. The dependent parameter
   * class does not store the input parameter themselves, but only
   * there values in a private array.
   */
  std::array<double, PARAM_NO> m_param_values;
  
  std::vector<std::unique_ptr<ReferenceUpdater>> m_updaters {};

  /* The two following methods represent the mecanism to loop over
   * the arbitrary number of variadic elements, the first one is called
   * as long as there is more than one elements in the list and the
   * second one is called when there is only one left.
   */
  template<typename First, typename ... Rest>
  void inputParameterLoop(First first, Rest... rest) {
    addParameterObserver(PARAM_NO - sizeof...(rest) - 1, first);
    inputParameterLoop(rest...);
  }

  template<typename Last>
  void inputParameterLoop(Last last) {
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
  void addParameterObserver(int i, Param param) {
    m_updaters.emplace_back(new ReferenceUpdater{
            param, this->m_param_values[i],
            ReferenceUpdater::PreAction{}, 
            [this](double){this->update(this->m_param_values[0]);}
    });
  }

};

template<typename ... Parameters>
std::unique_ptr<DependentParameter<Parameters...>> createDependentParameter(
    typename DependentParameter<Parameters...>::ValueCalculator value_calculator,
    std::shared_ptr<Parameters>... parameters) {
  return std::unique_ptr<DependentParameter<Parameters...>> {
    new DependentParameter<Parameters...> { value_calculator, parameters... }
  };
}

}

#endif  /* MODELFITTING_DEPENDENTPARAMETER_H */
