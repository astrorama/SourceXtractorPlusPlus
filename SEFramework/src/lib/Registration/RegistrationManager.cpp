/*
 * RegistrationManager.cpp
 *
 *  Created on: Jun 16, 2016
 *      Author: mschefer
 */

#include "SEFramework/Task/TaskRegistry.h"

#include "SEFramework/Registration/RegistrationManager.h"

namespace SExtractor {

RegistrationManager::RegistrationManager()
    : m_property_id_counter(0),
      m_task_registry(std::make_shared<TaskRegistry>()) {}

}
