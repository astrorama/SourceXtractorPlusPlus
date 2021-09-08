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
/*
 * CoordinateSystem.h
 *
 *  Created on: Nov 17, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_COORDINATESYSTEM_COORDINATESYSTEM_H_
#define _SEFRAMEWORK_COORDINATESYSTEM_COORDINATESYSTEM_H_

#include "ElementsKernel/Exception.h"
#include "SEUtils/PixelCoordinate.h"
#include <map>
#include <string>

namespace SourceXtractor {

struct WorldCoordinate {
  double m_alpha, m_delta;

  WorldCoordinate() : m_alpha(0), m_delta(0) {}
  WorldCoordinate(double alpha, double delta) : m_alpha(alpha), m_delta(delta) {}
};

// Note: (0,0) is the center of the first pixel

struct ImageCoordinate {
  double m_x, m_y;

  ImageCoordinate() : m_x(0), m_y(0) {}
  ImageCoordinate(double x, double y) : m_x(x), m_y(y) {}
};

class CoordinateSystem {
public:
  virtual ~CoordinateSystem() = default;

  virtual WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const = 0;
  virtual ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const = 0;

  virtual std::map<std::string, std::string> getFitsHeaders() const {
    return {};
  };
};

class InvalidCoordinatesException : public Elements::Exception {};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_COORDINATESYSTEM_COORDINATESYSTEM_H_ */
