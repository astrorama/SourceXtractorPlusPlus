/**
 * @file SEImplementation/Task/ExternalFlagTaskFactory.h
 * @date 06/17/16
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"
#include "SEFramework/Task/SourceTask.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagConfig.h"


namespace SExtractor {

/**
 * @class ExternalFlagTaskFactory
 * @brief
 *
 */
class ExternalFlagTaskFactory : public TaskFactory {
  
public:
  
  virtual ~ExternalFlagTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;
  
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
  
  void registerPropertyInstances(OutputRegistry&) override;

  
private:
  
  std::map<PropertyId, ExternalFlagConfig::FlagInfo> m_flag_info_map;
  std::vector<std::string> m_instance_names;
  
}; /* End of ExternalFlagTaskFactory class */

} /* namespace SExtractor */


#endif
