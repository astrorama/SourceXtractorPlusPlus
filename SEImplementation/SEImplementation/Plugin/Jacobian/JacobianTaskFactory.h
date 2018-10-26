/*
 * JacobianTaskFactory.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */


#ifndef _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class JacobianTaskFactory: public TaskFactory {
public:
  virtual ~JacobianTaskFactory() = default;

  std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASKFACTORY_H_
