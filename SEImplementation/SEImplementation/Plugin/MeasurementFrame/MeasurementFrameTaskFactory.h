/*
 * MeasurementFrameTaskFactory.h
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAMETASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAMETASKFACTORY_H_

#include <map>

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class MeasurementFrameTaskFactory : public TaskFactory {
public:
  MeasurementFrameTaskFactory() {}

  /// Destructor
  virtual ~MeasurementFrameTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  std::map<int, std::shared_ptr<MeasurementImageFrame>> m_measurement_frames;
};

}

#endif
