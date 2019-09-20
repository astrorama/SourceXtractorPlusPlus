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
 * @file SEImplementation/Task/PixelBoundariesTask.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_PIXELBOUNDARIESTASK_H
#define _SEIMPLEMENTATION_TASK_PIXELBOUNDARIESTASK_H

#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * @class PixelBoundariesTask
 * @brief Task to produce the PixelBoundaries Property
 *
 */
class PixelBoundariesTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundariesTask() = default;

  virtual void computeProperties(SourceInterface& source) const override;


private:

}; /* End of PixelBoundariesTask class */


/**
 * @class PixelBoundariesTaskHalfMaximum
 * @brief Task to produce the PixelBoundariesHalfMaximum Property
 *
 */
class PixelBoundariesTaskHalfMaximum : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundariesTaskHalfMaximum() = default;

  virtual void computeProperties(SourceInterface& source) const override;


private:

}; /* End of PixelBoundariesTaskHalfMaximum class */

} /* namespace SExtractor */


#endif
