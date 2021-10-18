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

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEINFO_MEASUREMENTFRAMEINFOTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEINFO_MEASUREMENTFRAMEINFOTASK_H_

#include "SEFramework/Task/SourceTask.h"

namespace SourceXtractor {

/**
 * @class MeasurementFrameInfoTask
  * @brief
*
 */
class MeasurementFrameInfoTask : public SourceTask {
public:

  /// Constructor
  explicit MeasurementFrameInfoTask(unsigned int instance) : m_instance(instance) {}

  virtual ~MeasurementFrameInfoTask() = default;


  void computeProperties(SourceInterface& source) const override;

private:
  unsigned int m_instance;

}; /* End of MeasurementFrameInfoTask class */

}


#endif /* _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEINFO_MEASUREMENTFRAMEINFOTASK_H_ */
