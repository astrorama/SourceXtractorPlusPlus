/**
 * @file BasicParameter.h
 * @date August 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_BASICPARAMETER_H
#define MODELFITTING_BASICPARAMETER_H

#include <functional>    // for std::function of the parameter observer
#include <vector>        // for vector of parameter values

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

  virtual ~BasicParameter();

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
  void addObserver(ParameterObserver observer);

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
  void setValue(const double new_value);

private:

  double m_value;
  std::vector<ParameterObserver> m_observer_list { };

};

}

#endif  /* MODELFITTING_BASICPARAMETER_H */
