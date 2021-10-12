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
 * @file SEImplementation/IsophotalFluxTask.h
 * @date 09/28/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_ISOPHOTALFLUXTASK_H
#define _SEIMPLEMENTATION_TASK_ISOPHOTALFLUXTASK_H

#include "SEFramework/Task/SourceTask.h"

namespace SourceXtractor {

/**
 * @class IsophotalFluxTask
 * @brief Task to produce the IsophotalFlux Property
 *
 */

class IsophotalFluxTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~IsophotalFluxTask() = default;

  explicit IsophotalFluxTask(SeFloat magnitude_zero_point) : m_magnitude_zero_point(magnitude_zero_point) {}

  virtual void computeProperties(SourceInterface& source) const override;


private:
  SeFloat m_magnitude_zero_point;

}; /* End of IsophotalFluxTask class */


} /* namespace SourceXtractor */


#endif
