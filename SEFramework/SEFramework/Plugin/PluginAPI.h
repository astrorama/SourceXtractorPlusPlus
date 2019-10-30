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
 * PluginAPI.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PLUGIN_PLUGINAPI_H_
#define _SEFRAMEWORK_PLUGIN_PLUGINAPI_H_

#include "Configuration/ConfigManager.h"
#include "SEFramework/Task/TaskFactoryRegistry.h"
#include "SEFramework/Output/OutputRegistry.h"

namespace SourceXtractor {

/**
 * @class PluginAPI
 * @brief This interface is given to the plugin to let it access object instances from the framework.
 */


class PluginAPI {
public:
  virtual ~PluginAPI() = default;

  virtual TaskFactoryRegistry& getTaskFactoryRegistry() const = 0;
  virtual OutputRegistry& getOutputRegistry() const = 0;
  virtual Euclid::Configuration::ConfigManager& getConfigManager() const = 0;
};

}

#endif /* _SEFRAMEWORK_PLUGIN_PLUGINAPI_H_ */
