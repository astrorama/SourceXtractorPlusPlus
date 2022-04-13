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
 * TaskFactoryRegistry.h
 *
 *  Created on: Aug 2, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_TASK_TASKFACTORYREGISTRY_H_
#define _SEFRAMEWORK_TASK_TASKFACTORYREGISTRY_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "SEFramework/Configuration/Configurable.h"

namespace SourceXtractor {

class OutputRegistry;
class TaskFactory;

class TaskFactoryRegistry : public Configurable {
public:

  /// Exception raised when trying to register 2 TaskFactories that produce the same PropertyType
  class DuplicateFactoryException : public Elements::Exception {
  public:
    DuplicateFactoryException() : Elements::Exception("Duplicate PropertyId in TaskProvider") {}
  };

  virtual ~TaskFactoryRegistry() = default;

  template<typename FactoryType, typename... Ts>
  void registerTaskFactory() {
    auto task_factory_shared = std::shared_ptr<TaskFactory>(std::unique_ptr<TaskFactory>(new FactoryType));

    m_task_factories.insert(task_factory_shared);

    registerTaskFactoryImpl<Ts...>(task_factory_shared);
  }

  const TaskFactory& getFactory(std::type_index type_id) const {
    return *m_type_task_factories_map.at(type_id);
  }

  // Configurable interface
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

  void registerPropertyInstances(OutputRegistry& output_registry);

private:
  template<typename T>
  void registerTaskFactoryImpl(std::shared_ptr<TaskFactory> task_factory) {
    auto type_index = std::type_index(typeid(T));
    // if we already have a factory for a property_id, throw an exception
    if (m_type_task_factories_map.find(type_index) != m_type_task_factories_map.end()) {
      throw DuplicateFactoryException();
    }
    m_type_task_factories_map[type_index] = task_factory;
  }

  template<typename T, typename T2, typename... Ts>
  void registerTaskFactoryImpl(std::shared_ptr<TaskFactory> task_factory) {
    registerTaskFactoryImpl<T>(task_factory);
    registerTaskFactoryImpl<T2, Ts...>(task_factory);
  }

  std::unordered_set<std::shared_ptr<TaskFactory>> m_task_factories;
  std::unordered_map<std::type_index, std::shared_ptr<TaskFactory>> m_type_task_factories_map;
};

}



#endif /* _SEFRAMEWORK_TASK_TASKFACTORYREGISTRY_H_ */
