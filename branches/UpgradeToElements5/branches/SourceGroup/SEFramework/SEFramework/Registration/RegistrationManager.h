/**
 * @file SEFramework/Registration/RegistrationManager.h
 * @date 06/14/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_REGISTRATION_REGISTRATIONMANAGER_H
#define _SEFRAMEWORK_REGISTRATION_REGISTRATIONMANAGER_H

#include <memory>

#include "SEFramework/Configuration/Configurable.h"

namespace SExtractor {

class TaskFactoryRegistry;
class TaskProvider;
class TaskFactory;

/**
 * @class RegistrationManager
 * @brief A singleton class used to register various things globally.
 *
 */
class RegistrationManager : public Configurable {
public:

  virtual ~RegistrationManager();

  // Removes copy/move constructors and assignment
  RegistrationManager(const RegistrationManager&) = delete;
  RegistrationManager(RegistrationManager&&) = delete;
  RegistrationManager& operator=(const RegistrationManager&) = delete;
  RegistrationManager& operator=(RegistrationManager&&) = delete;

  /// Gets a reference to the singleton instance
  static RegistrationManager& instance() {
    // Note: C++11 guarantees a thread-safe initialization
    static RegistrationManager instance;
    return instance;
  }

  std::shared_ptr<TaskFactoryRegistry> getTaskFactoryRegistry() const {
    return m_task_factory_registry;
  }

  /// Gets a pointer to the TaskProvider created by RegistrationManager to be used globally
  std::shared_ptr<TaskProvider> getTaskProvider() const {
    return m_task_provider;
  }

  // Implements the Configurable interface
  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  virtual void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  RegistrationManager();

  std::shared_ptr<TaskFactoryRegistry> m_task_factory_registry;
  std::shared_ptr<TaskProvider> m_task_provider;
}; /* End of RegistrationManager class */

} /* namespace SExtractor */


#endif
