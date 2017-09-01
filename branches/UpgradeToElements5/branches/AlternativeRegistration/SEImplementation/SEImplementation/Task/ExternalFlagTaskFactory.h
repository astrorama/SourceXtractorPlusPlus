/**
 * @file SEImplementation/Task/ExternalFlagTaskFactory.h
 * @date 06/17/16
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * @class ExternalFlagTaskFactory
 * @brief
 *
 */
class ExternalFlagTaskFactory : public TaskFactory {
  
public:
  
  virtual ~ExternalFlagTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;
  
  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) override;
  
  virtual const std::vector<PropertyId> getProducedProperties() override;
  
  void registerPropertyInstances(OutputRegistry&) override;

  
private:
  
  std::map<PropertyId, std::shared_ptr<SourceTask>> m_task_map;
  std::vector<std::string> m_instance_names;
  
}; /* End of ExternalFlagTaskFactory class */

} /* namespace SExtractor */


#endif
