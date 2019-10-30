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
/**
 * @file SEFramework/Task/GroupTask.h
 * @date 05/11/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_GROUPTASK_H
#define _SEFRAMEWORK_TASK_GROUPTASK_H

#include "SEFramework/Task/Task.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SourceXtractor {

/**
 * @class GroupTask
 * @brief A Task that acts on a SourceGroup to compute one or more properties
 *
 */
class GroupTask : public Task {

public:

  /**
   * @brief Destructor
   */
  virtual ~GroupTask() = default;

  /// Computes one or more properties for the SourceGroup and/or the Sources it contains
  virtual void computeProperties(SourceGroupInterface& group) const = 0;


private:

}; /* End of GroupTask class */

} /* namespace SourceXtractor */


#endif
