/**
 * @file SEFramework/Registration/RegistrationManager.h
 * @date 06/14/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_REGISTRATION_REGISTRATIONMANAGER_H
#define _SEFRAMEWORK_REGISTRATION_REGISTRATIONMANAGER_H

#include <memory>

namespace SExtractor {

class TaskRegistry;

/**
 * @class RegistrationManager
 * @brief
 *
 */
class RegistrationManager {
public:

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

private:
  RegistrationManager();

  unsigned int m_property_id_counter;
  std::shared_ptr<TaskRegistry> m_task_registry;

}; /* End of RegistrationManager class */

} /* namespace SEFramework */


#endif
