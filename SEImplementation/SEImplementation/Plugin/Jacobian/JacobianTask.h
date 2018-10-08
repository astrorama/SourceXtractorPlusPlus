/*
 * JacobianTask.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */


#ifndef _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASK_H_

#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

class JacobianTask : public GroupTask {
public:
  virtual ~JacobianTask() = default;

  JacobianTask(unsigned instance) : m_instance{instance} {}

  void computeProperties(SourceGroupInterface &source) const override;

private:
  unsigned m_instance;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASK_H_
