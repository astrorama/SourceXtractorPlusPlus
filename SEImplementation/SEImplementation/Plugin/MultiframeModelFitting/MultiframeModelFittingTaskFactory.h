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
/*
 * MultiframeModelFittingFactory.h
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGFACTORY_H_


#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFittingTask.h"


namespace SExtractor {

class MultiframeModelFittingTaskFactory : public TaskFactory {
public:
  MultiframeModelFittingTaskFactory() {}

  /// Destructor
  virtual ~MultiframeModelFittingTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  unsigned int m_max_iterations {0};
  std::vector<std::vector<int>> m_frame_indices_per_band;
};

}




#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGFACTORY_H_ */
