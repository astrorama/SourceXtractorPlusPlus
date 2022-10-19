/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEImplementation/Plugin/AssocMode/AssocModeTaskFactory.h"
#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"
#include "SEImplementation/Plugin/AssocMode/AssocMode.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeConfig.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeTask.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeDummyTask.h"
#include <NdArray/NdArray.h>
#include <iostream>
#include <sstream>

namespace SourceXtractor {

AssocModeTaskFactory::AssocModeTaskFactory() : m_assoc_mode(AssocModeConfig::AssocMode::UNKNOWN), m_assoc_radius(0.) {}

void AssocModeTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<AssocModeConfig>();
}

void AssocModeTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto config = manager.getConfiguration<AssocModeConfig>();

  m_catalogs               = config.getCatalogs();
  m_assoc_radius           = config.getAssocRadius();
  m_assoc_mode             = config.getAssocMode();
  m_add_property_instances = !config.getColumnsIdx().empty();
}

std::shared_ptr<Task> AssocModeTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == typeid(AssocMode)) {
    if (m_catalogs.empty()) {
      return std::make_shared<AssocModeDummyTask>();
    } else {
      return std::make_shared<AssocModeTask>(m_catalogs, m_assoc_mode, m_assoc_radius);
    }
  } else {
    return nullptr;
  }
}

void AssocModeTaskFactory::registerPropertyInstances(OutputRegistry& registry) {
  using Euclid::NdArray::NdArray;

  if (!m_add_property_instances) {
    return;
  }
  registry.registerColumnConverter<AssocMode, NdArray<SeFloat>>(
      "assoc_values", [](const AssocMode& prop) { return prop.getAssocValues(); }, "", "Assoc catalog values");
}

}  // namespace SourceXtractor
