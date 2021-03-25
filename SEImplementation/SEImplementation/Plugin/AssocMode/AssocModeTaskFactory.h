/*
 * AssocModeTaskFactory.h
 *
 *  Created on: Mar 5, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASKFACTORY_H_

#include "AssocModeConfig.h"
#include "SEFramework/Task/TaskFactory.h"


namespace SourceXtractor {

class AssocModeTaskFactory : public TaskFactory {
public:
  AssocModeTaskFactory();

  /// Destructor
  virtual ~AssocModeTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

private:
  std::vector<std::pair<std::string, unsigned int>> m_auto_names;

  AssocModeConfig::AssocMode m_assoc_mode;
  double m_assoc_radius;
  std::vector<AssocModeConfig::CatalogEntry> m_catalog;
};

}




#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASKFACTORY_H_ */
