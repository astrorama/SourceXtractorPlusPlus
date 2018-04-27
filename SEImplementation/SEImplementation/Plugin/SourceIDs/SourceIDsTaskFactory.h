/*
 * SourceIDsTaskFactory.h
 *
 *  Created on: Apr 27, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASKFACTORY_H_
#define __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/SourceIDs/SourceIDs.h"
#include "SEImplementation/Plugin/SourceIDs/SourceIDsTask.h"

namespace SExtractor {

class SourceIDsTaskFactory : public TaskFactory {
public:
  SourceIDsTaskFactory() {}

  /// Destructor
  virtual ~SourceIDsTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) {
      if (property_id == PropertyId::create<PartitionID>()) {
    return std::make_shared<SourceIDsTask>(m_magnitude_zero_point);
  } else {
    return nullptr;
  }

  void registerPropertyInstances(OutputRegistry&) override;

private:
};

}

#endif /* __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASKFACTORY_H_ */
