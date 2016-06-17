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

class TaskRegistry;
class TaskFactory;

/**
 * @class RegistrationManager
 * @brief A singleton class used to register various things globally.
 *
 */
class RegistrationManager : public Configurable {
public:

  virtual ~RegistrationManager();

  RegistrationManager(const RegistrationManager&) = delete;
  RegistrationManager(RegistrationManager&&) = delete;
  RegistrationManager& operator=(const RegistrationManager&) = delete;
  RegistrationManager& operator=(RegistrationManager&&) = delete;

  unsigned int getNextPropertyId() {
    return m_property_id_counter++;
  }

  static RegistrationManager& instance() {
    static RegistrationManager instance;
    return instance;
  }

  std::shared_ptr<TaskRegistry> getTaskRegistry() const {
    return m_task_registry;
  }

  void registerObject(std::unique_ptr<TaskFactory> task_factory);



  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) override;
  virtual void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  RegistrationManager();

  unsigned int m_property_id_counter;
  std::shared_ptr<TaskRegistry> m_task_registry;
  std::vector<std::unique_ptr<TaskFactory>> m_factory_list;

}; /* End of RegistrationManager class */

} /* namespace SEFramework */


#endif
