/**
 * @file SEutils/Observable.h
 * @date 05/20/16
 * @author mschefer
 */

#ifndef _SEUTILS_OBSERVABLE_H
#define _SEUTILS_OBSERVABLE_H

#include <memory>
#include <list>

namespace SEUtils {

template <class T>
class Observer {
public:
  virtual ~Observer() = default;
  virtual void handleMessage(const T& message) = 0;
};

/**
 * @class Observable
 * @brief
 *
 */

template<class T>
class Observable {

public:

  /**
   * @brief Destructor
   */
  virtual ~Observable() = default;

  void addObserver(std::shared_ptr<Observer<T>> observer) {
    m_observers.push_back(observer);
  }

  void removeObserver(std::shared_ptr<Observer<T>> observer) {
    m_observers.remove(observer);
  }

  void notifyObservers(const T& message) const {
    for (auto& observer : m_observers) {
      observer->handleMessage(message);
    }
  }

private:
  std::list<std::shared_ptr<Observer<T>>> m_observers;

}; /* End of Observable class */

} /* namespace SEUtils */


#endif
