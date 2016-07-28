/**
 * @file src/lib/Task/ExternalFlagTaskFactory.cpp
 * @date 06/17/16
 * @author nikoapos
 */

#include "SEFramework/Registration/AutoRegisterer.h"
#include "SEFramework/Registration/RegistrationManager.h"
#include "SEFramework/Output/OutputColumn.h"

#include "SEImplementation/Configuration/ExternalFlagConfig.h"
#include "SEImplementation/Property/ExternalFlag.h"
#include "SEImplementation/Task/ExternalFlagTask.h"
#include "SEImplementation/Task/ExternalFlagTaskFactory.h"

namespace SExtractor {

static AutoRegisterer<ExternalFlagTaskFactory> s_external_flag_registerer;

void ExternalFlagTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) {
  manager.registerConfiguration<ExternalFlagConfig>();
}


std::shared_ptr<Task> ExternalFlagTaskFactory::getTask(const PropertyId& property_id) {
  if (m_task_map.count(property_id) != 1) {
    return nullptr;
  }
  return m_task_map.at(property_id);
}


const std::vector<PropertyId> ExternalFlagTaskFactory::getProducedProperties() {
  std::vector<PropertyId> result{};
  for (auto& pair : m_task_map) {
    result.emplace_back(pair.first);
  }
  return result;
}


void ExternalFlagTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  // Loop through the different flag infos and create a task for each. The i
  // will be the index of the flag property.
  auto& flag_info_list = manager.getConfiguration<ExternalFlagConfig>().getFlagInfoList();
  for (unsigned int i = 0; i < flag_info_list.size(); ++i) {
    auto& pair = flag_info_list.at(i);
    m_instance_names.emplace_back(pair.first);
    auto property_id = PropertyId::create<ExternalFlag>(i);
    
    // Here we use an ugly switch for choosing the correct type of the task to
    // instantiate.
    switch (pair.second.second) {
      case ExternalFlagConfig::Type::OR:
      {
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskOr(pair.second.first, i)
        };
        break;
      }
      case ExternalFlagConfig::Type::AND:
      {
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskAnd(pair.second.first, i)
        };
        break;
      }
      case ExternalFlagConfig::Type::MIN:
      {
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskMin(pair.second.first, i)
        };
        break;
      }
      case ExternalFlagConfig::Type::MAX:
      {
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskMax(pair.second.first, i)
        };
        break;
      }
      case ExternalFlagConfig::Type::MOST:
      {
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskMost(pair.second.first, i)
        };
        break;
      }
    }
    
    // Register the catalog columns which can be produced from the ExternalFlag
    // property. These are two, one for the flag value and one for the count.
    OutputColumn::GetterFunction<ExternalFlag> flag_getter {[](const ExternalFlag& prop){return prop.getFlag();}};
    RegistrationManager::instance().registerOutputColumn(OutputColumn("IMAFLAGS_ISO_"+pair.first, flag_getter, i));
    OutputColumn::GetterFunction<ExternalFlag> count_getter {[](const ExternalFlag& prop){return prop.getCount();}};
    RegistrationManager::instance().registerOutputColumn(OutputColumn("NIMAFLAGS_ISO_"+pair.first, count_getter, i));
  }
}

void ExternalFlagTaskFactory::registerPropertyInstances(OutputRegistry& output_registry) {
  output_registry.registerPropertyInstances<ExternalFlag>(m_instance_names);
}

} // SExtractor namespace



