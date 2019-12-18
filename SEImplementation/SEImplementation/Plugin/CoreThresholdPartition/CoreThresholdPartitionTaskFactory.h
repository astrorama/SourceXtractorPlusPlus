/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file CoreThresholdPartitionTaskFactory.h
 *
 * @date May 27, 2019
 * @author mkuemmel@usm.lmu.de
 */
#ifndef _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDTASKFACTORY_H_

#include "CoreThresholdPartitionConfig.h"
#include "CoreThresholdPartitionTask.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SourceXtractor {
class CoreThresholdPartitionTaskFactory : public TaskFactory {
public:
  CoreThresholdPartitionTaskFactory() {}
  virtual ~CoreThresholdPartitionTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
    manager.registerConfiguration<CoreThresholdPartitionConfig>();
  };

  void configure(Euclid::Configuration::ConfigManager& manager) {
    auto snr_level_config = manager.getConfiguration<CoreThresholdPartitionConfig>();
    m_core_threshold = snr_level_config.getCoreThreshold();
  };

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const {
    if (property_id == PropertyId::create<NCorePixel>()) {
      return std::make_shared<CoreThresholdPartitionTask>(m_core_threshold);
    }
    else{
      return nullptr;
    }
  }

private:
  double m_core_threshold;
}; // end of CoreThresholdPartitionTaskFactory class
}  // namespace SExtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDTASKFACTORY_H_ */
