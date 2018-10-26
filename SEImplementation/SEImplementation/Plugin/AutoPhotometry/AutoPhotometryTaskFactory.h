/*
 * AutoPhotometryTaskFactory.h
 *
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

// TEMP
//#include "SEFramework/Image/Image.h"
//#include "SEFramework/Image/VectorImage.h"
//#include "SEFramework/Image/SubtractImage.h"
//#include "SEFramework/Image/WriteableImage.h"
//#include "SEFramework/Image/FitsWriter.h"

namespace SExtractor {

class AutoPhotometryTaskFactory : public TaskFactory {
public:
  AutoPhotometryTaskFactory() : m_magnitude_zero_point(0) {}

  /// Destructor
  virtual ~AutoPhotometryTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  void registerPropertyInstances(OutputRegistry &) override;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

private:
  SeFloat m_magnitude_zero_point;
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  bool    m_symmetry_usage;
  std::map<std::string, std::vector<unsigned int>> m_instances_per_group;
  std::vector<std::pair<std::string, unsigned int>> m_auto_names;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_ */
