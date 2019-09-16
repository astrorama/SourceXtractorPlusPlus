/**
 * @file src/lib/Task/ExternalFlagTaskFactory.cpp
 * @date 06/17/16
 * @author nikoapos
 */

#include "SEImplementation/Configuration/OutputConfig.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagConfig.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlag.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagTask.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagTaskFactory.h"

namespace SExtractor {

const char ExternalFlagTaskFactory::propertyName[] = "ExternalFlags";

void ExternalFlagTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ExternalFlagConfig>();
  manager.registerConfiguration<OutputConfig>();
}


std::shared_ptr<Task> ExternalFlagTaskFactory::createTask(const PropertyId& property_id) const {
  if (m_flag_info_map.find(property_id) != m_flag_info_map.end()) {
    auto& flag_info = m_flag_info_map.at(property_id);

    // Here we use an ugly switch for choosing the correct type of the task to
    // instantiate.
    switch (flag_info.second) {
      case ExternalFlagConfig::Type::OR:
        return std::make_shared<ExternalFlagTaskOr>(flag_info.first, property_id.getIndex());
      case ExternalFlagConfig::Type::AND:
        return std::make_shared<ExternalFlagTaskAnd>(flag_info.first, property_id.getIndex());
      case ExternalFlagConfig::Type::MIN:
        return std::make_shared<ExternalFlagTaskMin>(flag_info.first, property_id.getIndex());
      case ExternalFlagConfig::Type::MAX:
        return std::make_shared<ExternalFlagTaskMax>(flag_info.first, property_id.getIndex());
      case ExternalFlagConfig::Type::MOST:
        return std::make_shared<ExternalFlagTaskMost>(flag_info.first, property_id.getIndex());
    }
  }

  return nullptr;
}


void ExternalFlagTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  // Loop through the different flag infos. The i will be the index of the flag property.
  auto& flag_info_list = manager.getConfiguration<ExternalFlagConfig>().getFlagInfoList();
  for (unsigned int i = 0; i < flag_info_list.size(); ++i) {
    auto& pair = flag_info_list.at(i);
    m_instance_names.emplace_back(pair.first, i);
    auto property_id = PropertyId::create<ExternalFlag>(i);
    
    m_flag_info_map[property_id] = pair.second;
  }

  auto& output_config = manager.getConfiguration<OutputConfig>();
  auto& output_properties = output_config.getOutputProperties();
  m_is_output_requested = std::find(output_properties.begin(), output_properties.end(), propertyName)
      != output_properties.end();
}

void ExternalFlagTaskFactory::registerPropertyInstances(OutputRegistry& output_registry) {
  output_registry.registerPropertyInstances<ExternalFlag>(m_instance_names);
  if (m_is_output_requested && m_instance_names.size() <= 0) {
    throw Elements::Exception() << "Requested property ExternalFlag is not configured to produce any output";
  }
}

} // SExtractor namespace



