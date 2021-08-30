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
 * TransformedAperture.cpp
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez
 */

#include "SEFramework/Aperture/TransformedAperture.h"
#include <algorithm>

namespace SourceXtractor {

TransformedAperture::TransformedAperture(std::shared_ptr<Aperture> decorated,
                                         const std::tuple<double, double, double, double> &jacobian)
  : m_decorated{decorated} {

  m_transform[0] = std::get<0>(jacobian);
  m_transform[1] = std::get<1>(jacobian);
  m_transform[2] = std::get<2>(jacobian);
  m_transform[3] = std::get<3>(jacobian);

  double inv_det = 1. / (m_transform[0] * m_transform[3] - m_transform[2] * m_transform[1]);

  m_inv_transform[0] = m_transform[3] * inv_det;
  m_inv_transform[1] = -m_transform[1] * inv_det;
  m_inv_transform[2] = -m_transform[2] * inv_det;
  m_inv_transform[3] = m_transform[0] * inv_det;
}

inline std::pair<double, double> transform(int x, int y, const std::array<double, 4> &t) {
  return {
    x * t[0] + y * t[2],
    x * t[1] + y * t[3]
  };
}

PixelCoordinate TransformedAperture::getMinPixel(SeFloat x, SeFloat y) const {
  auto min = m_decorated->getMinPixel(0, 0);
  auto max = m_decorated->getMaxPixel(0, 0);

  auto a = transform(min.m_x, min.m_y, m_transform);
  auto b = transform(max.m_x, min.m_y, m_transform);
  auto c = transform(min.m_x, max.m_y, m_transform);
  auto d = transform(max.m_x, max.m_y, m_transform);

  auto min_x = std::min({a.first, b.first, c.first, d.first});
  auto min_y = std::min({a.second, b.second, c.second, d.second});

  return PixelCoordinate(x + min_x, y + min_y);
}

PixelCoordinate TransformedAperture::getMaxPixel(SeFloat x, SeFloat y) const {
  auto min = m_decorated->getMinPixel(0, 0);
  auto max = m_decorated->getMaxPixel(0, 0);

  auto a = transform(min.m_x, min.m_y, m_transform);
  auto b = transform(max.m_x, min.m_y, m_transform);
  auto c = transform(min.m_x, max.m_y, m_transform);
  auto d = transform(max.m_x, max.m_y, m_transform);

  auto min_x = std::max({a.first, b.first, c.first, d.first});
  auto min_y = std::max({a.second, b.second, c.second, d.second});

  return PixelCoordinate(x + min_x, y + min_y);
}

SeFloat TransformedAperture::getArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const {
  auto diff_x = pixel_x - center_x;
  auto diff_y = pixel_y - center_y;

  SeFloat new_diff_x = diff_x * m_inv_transform[0] + diff_y * m_inv_transform[2];
  SeFloat new_diff_y = diff_x * m_inv_transform[1] + diff_y * m_inv_transform[3];

  return m_decorated->getArea(0, 0, new_diff_x, new_diff_y);
}

SeFloat TransformedAperture::drawArea(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const {
  auto diff_x = pixel_x - center_x;
  auto diff_y = pixel_y - center_y;

  SeFloat new_diff_x = diff_x * m_inv_transform[0] + diff_y * m_inv_transform[2];
  SeFloat new_diff_y = diff_x * m_inv_transform[1] + diff_y * m_inv_transform[3];

  return m_decorated->drawArea(0, 0, new_diff_x, new_diff_y);
}

SeFloat TransformedAperture::getRadiusSquared(SeFloat center_x, SeFloat center_y, SeFloat pixel_x, SeFloat pixel_y) const {
  auto diff_x = pixel_x - center_x;
  auto diff_y = pixel_y - center_y;

  SeFloat new_diff_x = diff_x * m_inv_transform[0] + diff_y * m_inv_transform[2];
  SeFloat new_diff_y = diff_x * m_inv_transform[1] + diff_y * m_inv_transform[3];

  return m_decorated->getRadiusSquared(0, 0, new_diff_x, new_diff_y);
}

} // end SourceXtractor
