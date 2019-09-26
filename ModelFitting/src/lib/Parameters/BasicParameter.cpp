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
