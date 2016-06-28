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
  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) = 0;
  
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

