/** Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * @file src/lib/Pipeline/Deblending.cpp
 * @date 05/26/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Deblending.h"

namespace SourceXtractor {

Deblending::Deblending(std::vector<std::shared_ptr<DeblendStep>> deblend_steps)
  : m_deblend_steps(std::move(deblend_steps)) {
}

void Deblending::receiveSource(const std::shared_ptr<SourceGroupInterface>& group) {

  // Applies every DeblendStep to the SourceGroup
  for (auto& step : m_deblend_steps) {
    step->deblend(*group);
  }

  // If the SourceGroup still contains sources, we notify the observers
  if (group->begin() != group->end()) {
    sendSource(group);
  }
}

std::set<PropertyId> Deblending::requiredProperties() const {
  std::set<PropertyId> properties;
  for (auto& step : m_deblend_steps) {
    auto step_props = step->requiredProperties();
    std::copy(step_props.begin(), step_props.end(), std::inserter(properties, properties.end()));
  }
  return properties;
}

void Deblending::receiveProcessSignal(const ProcessSourcesEvent& event) {
  sendProcessSignal(event);
}

} // SEFramework namespace
