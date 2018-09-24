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
  AperturePhotometryTaskFactory() : m_magnitude_zero_point(0), m_symmetry_usage(true) {}

  /// Destructor
  virtual ~AperturePhotometryTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

  void registerPropertyInstances(OutputRegistry&) override;

private:

  std::vector<SeFloat> m_apertures;
  std::vector<unsigned int> m_image_instances;

  SeFloat m_magnitude_zero_point;
  bool    m_symmetry_usage;
  std::map<std::pair<unsigned int, SeFloat>, unsigned int> m_aperture_instances;

  std::vector<std::pair<std::string, unsigned int>> m_instance_names;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_ */
