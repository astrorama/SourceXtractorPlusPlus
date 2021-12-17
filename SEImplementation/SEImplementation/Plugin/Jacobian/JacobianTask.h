/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

namespace SourceXtractor {

class JacobianGroupTask : public GroupTask {
public:
  virtual ~JacobianGroupTask() = default;

  explicit JacobianGroupTask(unsigned instance) : m_instance{instance} {}

  void computeProperties(SourceGroupInterface &source) const override;

private:
  unsigned m_instance;
};

class JacobianSourceTask : public SourceTask {
public:
  virtual ~JacobianSourceTask() = default;

  explicit JacobianSourceTask(unsigned instance) : m_instance{instance} {}

  void computeProperties(SourceInterface &source) const override;

private:
  unsigned m_instance;
};

} // end SourceXtractor

#endif // _SEIMPLEMENTATION_PLUGIN_JACOBIAN_JACOBIANTASK_H_
