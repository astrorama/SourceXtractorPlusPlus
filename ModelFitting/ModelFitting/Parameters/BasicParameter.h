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

  BasicParameter(const BasicParameter&) = delete;
  BasicParameter& operator=(const BasicParameter&) = delete;
  BasicParameter(BasicParameter&&) = delete;
  BasicParameter& operator=(BasicParameter&&) = delete;

  /*
   * @brief Getter to access the private parameter value
   */
  virtual double getValue() const {
    return m_value;
  }

  /**
   * @brief
   *    The addObserver method adding observer to the member
   *    list
   * @return The key to use for removing an observer
   */
  std::size_t addObserver(ParameterObserver observer);
  
  bool removeObserver(std::size_t id);

  bool isObserved() const;

protected:
  typedef std::function<void(void)> GetValueHook;

  GetValueHook m_get_value_hook;

  explicit BasicParameter(const double value) :
      m_value(value) {
  }

  /*
   * @brief Setter for the new value, which also notify the
   * observers, by calling them with the new value.
   */
  virtual void setValue(const double new_value);

protected:
  double m_value;

private:
  std::map<std::size_t, ParameterObserver> m_observer_map;
  std::size_t m_last_obs_id = 0;
};

}

#endif  /* MODELFITTING_BASICPARAMETER_H */
