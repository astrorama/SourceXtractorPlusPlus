/**
 * @file BasicParameter.cpp
 *
 * Created on: August 12, 2015
 *     Author: Pierre Dubath
 */

#include "ModelFitting/Parameters/BasicParameter.h"

namespace ModelFitting {

using namespace std;

BasicParameter::~BasicParameter() = default;

void BasicParameter::setValue(const double new_value) {
  *m_value = new_value;
  for (auto& observer : *m_observer_map) {
    observer.second(*m_value);
  }
}

std::size_t BasicParameter::addObserver(ParameterObserver observer) {
  *m_last_obs_id += 1;
  m_observer_map->emplace(*m_last_obs_id, std::move(observer));
  return *m_last_obs_id;
}

bool BasicParameter::removeObserver(std::size_t id) {
  return m_observer_map->erase(id);
}

bool BasicParameter::isObserved() const {
  return !m_observer_map->empty();
}

}// namespace ModelFitting
