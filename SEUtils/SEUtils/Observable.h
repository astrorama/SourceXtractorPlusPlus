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
 * @file SEutils/Observable.h
 * @date 05/20/16
 * @author mschefer
 */

#ifndef _SEUTILS_OBSERVABLE_H
#define _SEUTILS_OBSERVABLE_H

#include <memory>
#include <list>

namespace SExtractor {

/**
 * @class Observer
 * @brief Observer interface to be used  with Observable to implement the Observer pattern.
 *
 */

template <class T>
class Observer {
public:
  virtual ~Observer() = default;
  virtual void handleMessage(const T& message) = 0;
};

/**
 * @class Observable
 * @brief Implements the Observer pattern. Notifications will be made using a message of type T.
 *
 */

template<class T>
class Observable {

public:

  /// Destructor
  virtual ~Observable() = default;

  /// Adds an Observer that will be notified when notify Observers is called
  virtual void addObserver(std::shared_ptr<Observer<T>> observer) {
    m_observers.push_back(observer);
  }

  /// Removes a previously added Observer from the list of Observers to notify
  virtual void removeObserver(std::shared_ptr<Observer<T>> observer) {
    m_observers.remove(observer);
  }
  
protected:

  // Notify all registered Observers with the given message
  void notifyObservers(const T& message) const {
    for (auto& observer : m_observers) {
      observer->handleMessage(message);
    }
  }

private:
  std::list<std::shared_ptr<Observer<T>>> m_observers;

}; /* End of Observable class */

} /* namespace SExtractor */


#endif
