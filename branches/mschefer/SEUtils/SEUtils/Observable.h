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
