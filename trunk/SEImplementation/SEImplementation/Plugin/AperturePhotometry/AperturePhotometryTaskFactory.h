/*
 * AperturePhotometryTaskFactory.h
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class AperturePhotometryTaskFactory : public TaskFactory {
public:
  AperturePhotometryTaskFactory() {}

  /// Destructor
  virtual ~AperturePhotometryTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

  void registerPropertyInstances(OutputRegistry&) override;

private:
  std::vector<std::string> m_instance_names;
  std::vector<SeFloat> m_apertures;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_ */
