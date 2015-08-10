/**
 * @file BasicParameter.cpp
 *
 * Created on: July 28, 2015
 *     Author: Pierre Dubath
 */

#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "ModelFitting/Parameters/BasicParameter.h"

namespace ModelFitting {

using namespace std;

void BasicParameter::updateValueAndNotifyWatcher(const double new_value) {
  m_value = new_value;
  // Notify the watchers with the new value
  for (auto const& watcher_ptr : m_watcher_ptr_vector) {
    watcher_ptr->updatedValue(new_value);
  }
}

void BasicParameter::addWatcher(std::unique_ptr<ParameterWatcher> watcher_ptr) {
  m_watcher_ptr_vector.push_back( move(watcher_ptr) );
}

}// namespace ModelFitting
