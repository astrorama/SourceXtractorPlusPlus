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

#ifndef _SEIMPLEMENTATION_PLUGIN_REFERENCECOORDINATES_REFERENCECOORDINATES_H_
#define _SEIMPLEMENTATION_PLUGIN_REFERENCECOORDINATES_REFERENCECOORDINATES_H_

#include "SEFramework/Property/Property.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SourceXtractor {

class ReferenceCoordinates : public Property {

public:
  virtual ~ReferenceCoordinates() = default;

  explicit ReferenceCoordinates(std::shared_ptr<CoordinateSystem> coordinate_system)
      : m_coordinate_system(coordinate_system) {}

  const std::shared_ptr<CoordinateSystem> getCoordinateSystem() const {
    return m_coordinate_system;
  }

private:
  std::shared_ptr<CoordinateSystem> m_coordinate_system;

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_REFERENCECOORDINATES_REFERENCECOORDINATES_H_ */
