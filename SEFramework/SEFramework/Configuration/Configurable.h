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
 * @file SEFramework/Configuration/Configurable.h
 * @date 06/16/16
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_CONFIGURATION_CONFIGURABLE_H
#define _SEFRAMEWORK_CONFIGURATION_CONFIGURABLE_H

#include "Configuration/ConfigManager.h"

namespace SExtractor {

/**
 * @brief Interface of objects which can be configured
 * @details
 * This interface is introduced to allow the dynamic determination of the available
 * program options, so introduction of new tasks or component implementations
 * will not require modifications of other parts of the system.
 */
class Configurable {
  
public:
  
  /// Destructor
  virtual ~Configurable() = default;

  /**
   * @brief Registers all the Configuration dependencies
   * @details
   * Configurable implementations should implement this method to register
   * to the given ConfigManager (using the registerConfiguration() method)
   * all the Configurations they are going to use
   * @param manager The manager to register the dependencies to
   */
  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const = 0;
  
  /**
   * @brief Method which should initialize the object
   * @details
   * The given manager is guaranteed to already be in the initialized state, so
   * all the required Configurations can be accessed using its getConfiguration()
   * method.
   * @param manager The manager to get the Configurations from
   */
  virtual void configure(Euclid::Configuration::ConfigManager& manager) = 0;
  
};

} /* namespace SExtractor */

#endif /* _SEFRAMEWORK_CONFIGURATION_CONFIGURABLE_H */

