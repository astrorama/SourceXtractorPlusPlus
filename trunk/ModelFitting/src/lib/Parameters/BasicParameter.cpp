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
  m_value = new_value;
  for (auto& observer : m_observer_list) {
    observer(m_value);
  }
}

void BasicParameter::addObserver(ParameterObserver observer) {
  m_observer_list.emplace_back(std::move(observer));
}

}// namespace ModelFitting
