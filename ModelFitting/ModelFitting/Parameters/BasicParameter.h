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

  BasicParameter(const BasicParameter&) = default;
  BasicParameter& operator=(const BasicParameter&) = default;
  BasicParameter(BasicParameter&&) = default;
  BasicParameter& operator=(BasicParameter&&) = default;

  /*
   * @brief Getter to access the private parameter value
   */
  double getValue() const {
    /**
     * This is a terrible hack to allow DependentParameter to be evaluated lazily while respecting
     * the API of ModelFitting. BasicParameter can, and is, passed around by value, which implies we need to
     * reinvent the "virtual wheel" if we want to change the behaviour of this method.
     * Models do not rely on getValue, as they use ReferenceUpdater instead, so this hack should not affect
     * the performance of the model fitting.
     */
    if (m_get_value_hook) {
      m_get_value_hook();
    }
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

  bool isObserved() const;

protected:
  typedef std::function<void(void)> GetValueHook;

  GetValueHook m_get_value_hook;

  BasicParameter(const double value) :
      m_value {new double{value}} {
  }

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
