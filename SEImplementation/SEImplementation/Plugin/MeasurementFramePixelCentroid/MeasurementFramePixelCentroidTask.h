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
 * @file SEImplementation/Task/PixelCentroidTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_MEASUREMENTFRAMEPIXELCENTROIDTASK_H
#define _SEIMPLEMENTATION_TASK_MEASUREMENTFRAMEPIXELCENTROIDTASK_H

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * @class MeasurementFramePixelCentroidTask
 * @brief Task to produce the PixelCentroid Property
 *
 */

class MeasurementFramePixelCentroidTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~MeasurementFramePixelCentroidTask() = default;

  MeasurementFramePixelCentroidTask(unsigned int instance) : m_instance(instance) {}

  virtual void computeProperties(SourceInterface& source) const override;


private:
  unsigned int m_instance;

}; /* End of MeasurementFramePixelCentroidTask class */


} /* namespace SExtractor */


#endif
