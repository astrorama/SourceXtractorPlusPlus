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
/*
 * TaskFactoryRegistry.cpp
 *
 *  Created on: Aug 3, 2016
 *      Author: mschefer
 */

#include "SEFramework/Task/TaskFactoryRegistry.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

void TaskFactoryRegistry::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  // Reports the config dependencies for every TaskFactory that was registered
  for (auto& factory : m_task_factories) {
    factory->reportConfigDependencies(manager);
  }
}

void TaskFactoryRegistry::configure(Euclid::Configuration::ConfigManager& manager) {
  for (auto& factory : m_task_factories) {
    factory->configure(manager);
  }
}

void TaskFactoryRegistry::registerPropertyInstances(OutputRegistry& output_registry) {
  for (auto& factory : m_task_factories) {
    factory->registerPropertyInstances(output_registry);
  }
}

}
