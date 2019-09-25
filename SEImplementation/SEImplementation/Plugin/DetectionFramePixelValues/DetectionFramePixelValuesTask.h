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
 * @file SEImplementation/Task/DetectionFramePixelValuesTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASK_H
#define _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASK_H

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class DetectionFramePixelValuesTask
 * @brief Task to produce the DetectionFramePixelValues Property
 *
 */

class DetectionFramePixelValuesTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFramePixelValuesTask() = default;

  DetectionFramePixelValuesTask() {}

  virtual void computeProperties(SourceInterface& source) const override;

}; /* End of DetectionFramePixelValuesTask class */



} /* namespace SExtractor */


#endif
