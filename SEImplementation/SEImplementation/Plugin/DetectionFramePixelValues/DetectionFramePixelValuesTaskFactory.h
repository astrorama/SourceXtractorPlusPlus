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
 * @file SEImplementation/Task/DetectionFramePixelValuesTaskFactory.h
 * @date 06/16/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_DETECTIONFRAMEPIXELVALUESTASKFACTORY_H

#include <memory>
#include <vector>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SourceXtractor {

/**
 * @class DetectionFramePixelValuesTaskFactory
 * @brief TaskFactory to produce DetectionFramePixelValuesTask
 *
 */
class DetectionFramePixelValuesTaskFactory : public TaskFactory {
public:

  /// Destructor
  virtual ~DetectionFramePixelValuesTaskFactory() = default;

  // TaskFactory implementation
  std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
};

} /* namespace SourceXtractor */


#endif
