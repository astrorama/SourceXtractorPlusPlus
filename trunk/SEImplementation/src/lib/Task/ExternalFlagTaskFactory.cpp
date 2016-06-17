/**
 * @file src/lib/Task/ExternalFlagTaskFactory.cpp
 * @date 06/17/16
 * @author nikoapos
 */

#include "SEImplementation/Configuration/ExternalFlagConfig.h"
#include "SEImplementation/Property/ExternalFlag.h"
#include "SEImplementation/Task/ExternalFlagTask.h"
#include "SEImplementation/Task/ExternalFlagTaskFactory.h"

namespace SExtractor {


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
  auto& flag_info_list = manager.getConfiguration<ExternalFlagConfig>().getFlagInfoList();
  for (unsigned int i = 0; i < flag_info_list.size(); ++i) {
    auto& pair = flag_info_list.at(i);
    switch (pair.second.second) {
      case ExternalFlagConfig::Type::OR:
      {
        auto property_id = PropertyId::create<ExternalFlag>(i);
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskOr(pair.second.first, i)
        };
        break;
      }
      case ExternalFlagConfig::Type::AND:
      {
        auto property_id = PropertyId::create<ExternalFlag>(i);
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskAnd(pair.second.first, i)
        };
        break;
      }
      case ExternalFlagConfig::Type::MIN:
      {
        auto property_id = PropertyId::create<ExternalFlag>(i);
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskMin(pair.second.first, i)
        };
        break;
      }
      case ExternalFlagConfig::Type::MAX:
      {
        auto property_id = PropertyId::create<ExternalFlag>(i);
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskMax(pair.second.first, i)
        };
        break;
      }
      case ExternalFlagConfig::Type::MOST:
      {
        auto property_id = PropertyId::create<ExternalFlag>(i);
        m_task_map[property_id] = std::shared_ptr<SourceTask>{
          new ExternalFlagTaskMost(pair.second.first, i)
        };
        break;
      }
    }
  }
}


} // SExtractor namespace



