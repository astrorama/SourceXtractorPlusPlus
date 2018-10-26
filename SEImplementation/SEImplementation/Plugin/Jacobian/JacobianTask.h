/*
 * JacobianTask.h
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */


#ifndef _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

class JacobianGroupTask : public GroupTask {
public:
  virtual ~JacobianGroupTask() = default;

  JacobianGroupTask(unsigned instance) : m_instance{instance} {}

  void computeProperties(SourceGroupInterface &source) const override;

private:
  unsigned m_instance;
};

class JacobianSourceTask : public SourceTask {
public:
  virtual ~JacobianSourceTask() = default;

  JacobianSourceTask(unsigned instance) : m_instance{instance} {}

  void computeProperties(SourceInterface &source) const override;

private:
  unsigned m_instance;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASK_H_
