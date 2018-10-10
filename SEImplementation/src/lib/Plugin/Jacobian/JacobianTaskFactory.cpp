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
  if (property_id.getTypeId() == typeid(JacobianGroup)) {
    return std::make_shared<JacobianGroupTask>(property_id.getIndex());
  }
  else if (property_id.getTypeId() == typeid(JacobianSource)) {
    return std::make_shared<JacobianSourceTask>(property_id.getIndex());
  }
  return nullptr;
}

} // end SExtractor
