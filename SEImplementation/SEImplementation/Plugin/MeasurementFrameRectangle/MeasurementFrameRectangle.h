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
 * MeasurementFrameRectangle.h
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLE_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLE_H_

#include <tuple>

#include "SEUtils/PixelRectangle.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"

namespace SourceXtractor {

class MeasurementFrameRectangle: public Property {
public:
  virtual ~MeasurementFrameRectangle() = default;

  explicit MeasurementFrameRectangle(bool bad_projection):
  m_min_coord{-1, -1}, m_max_coord{-1, -1}, m_bad_projection{bad_projection} {}

  MeasurementFrameRectangle(ImageCoordinate min_coord, ImageCoordinate max_coord):
      m_min_coord(min_coord), m_max_coord(max_coord), m_bad_projection{false} {
    assert(min_coord.m_x <= max_coord.m_x && min_coord.m_y <= max_coord.m_y);
  }

  ImageCoordinate getTopLeft() const {
    assert(m_max_coord.m_x >= 0.0);
    return m_min_coord;
  }

  ImageCoordinate getBottomRight() const {
    assert(m_max_coord.m_x >= 0.0);
    return m_max_coord;
  }

  double getWidth() const {
    if (m_max_coord.m_x < 0.0)
      return 0;
    return m_max_coord.m_x - m_min_coord.m_x;
  }

  double getHeight() const {
    if (m_max_coord.m_x < 0.0)
      return 0;
    return m_max_coord.m_y - m_min_coord.m_y;
  }

  std::tuple<ImageCoordinate, ImageCoordinate> getImageRect() const {
    return std::make_tuple(m_min_coord, m_max_coord);
  }

  bool badProjection() const {
    return m_bad_projection;
  }

  bool isValid() const {
    return !m_bad_projection && m_max_coord.m_x >= 0.0 && m_max_coord.m_y >= 0.0 && getWidth() > 0.0 && getHeight() > 0.0;
  }

private:
  ImageCoordinate m_min_coord, m_max_coord;
  bool m_bad_projection;
};

} // end SourceXtractor

#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLE_H_
