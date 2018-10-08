/*
 * JacobianTaskFactory.cpp
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/Jacobian/JacobianTaskFactory.h"
#include "SEImplementation/Plugin/Jacobian/JacobianTask.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"

namespace SExtractor {

std::shared_ptr<Task> JacobianTaskFactory::createTask(const SExtractor::PropertyId &property_id) const {
  if (property_id.getTypeId() == typeid(Jacobian)) {
    return std::make_shared<JacobianTask>(property_id.getIndex());
  }
  return nullptr;
}

} // end SExtractor
