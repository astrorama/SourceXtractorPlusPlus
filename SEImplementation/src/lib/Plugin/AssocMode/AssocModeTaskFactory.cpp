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


#include <SEImplementation/Plugin/AssocMode/AssocMode.h>
#include <iostream>
#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"

#include "SEImplementation/Plugin/AssocMode/AssocModeTask.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeTaskFactory.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeConfig.h"


namespace SourceXtractor {

AssocModeTaskFactory::AssocModeTaskFactory() {}

void AssocModeTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<AssocModeConfig>();
}

void AssocModeTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto config = manager.getConfiguration<AssocModeConfig>();

  m_catalog = config.getCatalog();
  m_assoc_radius = config.getAssocRadius();
  m_assoc_mode = config.getAssocMode();
}

std::shared_ptr<Task> AssocModeTaskFactory::createTask(const PropertyId &property_id) const {
  if (property_id.getTypeId() == typeid(AssocMode)) {
    return std::make_shared<AssocModeTask>(m_catalog, m_assoc_mode, m_assoc_radius);
  } else {
    return nullptr;
  }
}

}
