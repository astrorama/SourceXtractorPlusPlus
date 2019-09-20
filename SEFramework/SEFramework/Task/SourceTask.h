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
 * @file SEFramework/Task/SourceTask.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_TASK_SOURCETASK_H
#define _SEFRAMEWORK_TASK_SOURCETASK_H

#include "SEFramework/Task/Task.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

/**
 * @class SourceTask
 * @brief A Task that acts on a Source to compute one or more properties
 *
 */
class SourceTask : public Task {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceTask() = default;

  /// Computes one or more properties for the Source
  virtual void computeProperties(SourceInterface& source) const = 0;

private:

}; /* End of SourceTask class */

} /* namespace SExtractor */


#endif
