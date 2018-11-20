/*
 * AperturePhotometryTaskFactory.h
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEUtils/Types.h"

namespace SExtractor {

class AperturePhotometryTaskFactory : public TaskFactory {
public:
  AperturePhotometryTaskFactory() : m_magnitude_zero_point(0), m_symmetry_usage(true) {}

  /// Destructor
  virtual ~AperturePhotometryTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const override;

  void configure(Euclid::Configuration::ConfigManager &manager) override;

  virtual std::shared_ptr<Task> createTask(const PropertyId &property_id) const override;

  void registerPropertyInstances(OutputRegistry &) override;

private:
  SeFloat m_magnitude_zero_point;
  bool m_symmetry_usage;

  // Apertures for a given image ID
  std::map<int, std::vector<float>> m_aperture_config;
  // All configured apertures (required to be able to flag on the detection image)
  std::vector<float> m_all_apertures;
  // Image IDs that correspond to a output column name
  std::map<std::string, std::vector<int>> m_outputs;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_ */
