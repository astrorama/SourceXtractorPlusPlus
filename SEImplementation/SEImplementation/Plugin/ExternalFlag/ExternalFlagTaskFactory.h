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
 * @file SEImplementation/Task/ExternalFlagTaskFactory.h
 * @date 06/17/16
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"
#include "SEFramework/Task/SourceTask.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagConfig.h"


namespace SourceXtractor {

/**
 * @class ExternalFlagTaskFactory
 * @brief
 *
 */
class ExternalFlagTaskFactory : public TaskFactory {
  
public:
  static const char propertyName[];

  virtual ~ExternalFlagTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;
  
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
  
  void registerPropertyInstances(OutputRegistry&) override;

  
private:
  
  std::map<PropertyId, ExternalFlagConfig::FlagInfo> m_flag_info_map;
  std::vector<std::pair<std::string, unsigned int>> m_instance_names;
  
  bool m_is_output_requested = false;

}; /* End of ExternalFlagTaskFactory class */

} /* namespace SourceXtractor */


#endif
