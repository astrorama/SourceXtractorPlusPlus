/**
 * @file BasicParameter.h
 * @date August 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_BASICPARAMETER_H
#define MODELFITTING_BASICPARAMETER_H

#include <functional>    // for std::function of the parameter observer
#include <map>           // for std::map
#include <memory>

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
    return *m_value;
  }

  /**
   * @brief
   *    The addObserver method adding observer to the member
   *    list
   * @return The key to use for removing an observer
   */
  std::size_t addObserver(ParameterObserver observer);
  
  bool removeObserver(std::size_t id);

protected:

  BasicParameter(const double value) :
      m_value {new double{value}} {
  }

  BasicParameter(const BasicParameter&) = default;
  BasicParameter& operator=(const BasicParameter&) = default;
  BasicParameter(BasicParameter&&) = default;
  BasicParameter& operator=(BasicParameter&&) = default;

  /*
   * @brief Setter for the new value, which also notify the
   * observers, by calling them with the new value.
   */
  void setValue(const double new_value);

private:

  std::shared_ptr<double> m_value;
  std::shared_ptr<std::map<std::size_t, ParameterObserver>> m_observer_map {
                                    new std::map<std::size_t, ParameterObserver>{}};
  std::shared_ptr<std::size_t> m_last_obs_id {new std::size_t{0}};

};

}

#endif  /* MODELFITTING_BASICPARAMETER_H */
