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

#ifndef _SEIMPLEMENTATION_PLUGIN_REFERENCECOORDINATES_REFERENCECOORDINATESTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_REFERENCECOORDINATES_REFERENCECOORDINATESTASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SourceXtractor {

/**
 * @class ReferenceCoordinatesTask
  * @brief
*
 */
class ReferenceCoordinatesTask : public SourceTask {
public:

  /**
   * @brief Destructor
   */
  virtual ~ReferenceCoordinatesTask() = default;

  /// Constructor
  explicit ReferenceCoordinatesTask(unsigned int instance, std::shared_ptr<CoordinateSystem> coordinate_system)
      : m_instance(instance), m_coordinate_system(coordinate_system) {
  }

  void computeProperties(SourceInterface& source) const override;

private:
  unsigned int m_instance;
  std::shared_ptr<CoordinateSystem> m_coordinate_system;

}; /* End of ReferenceCoordinatesTask class */

}


#endif /* _SEIMPLEMENTATION_PLUGIN_REFERENCECOORDINATES_REFERENCECOORDINATESTASK_H_ */
