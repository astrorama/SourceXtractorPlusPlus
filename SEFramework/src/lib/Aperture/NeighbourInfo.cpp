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
 * NeighbourInfo.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: Alejandro Alvarez
 */

#include "SEFramework/Aperture/NeighbourInfo.h"

namespace SExtractor {

NeighbourInfo::NeighbourInfo(const PixelCoordinate &min_pixel, const PixelCoordinate &max_pixel,
                             const std::vector<SExtractor::PixelCoordinate> &pixel_list,
                             const std::shared_ptr<SExtractor::Image<SExtractor::SeFloat>> &threshold_image)
  : m_offset{min_pixel} {
  auto width = max_pixel.m_x - min_pixel.m_x + 1;
  auto height = max_pixel.m_y - min_pixel.m_y + 1;
  m_neighbour_image = VectorImage<int>::create(width, height);

  for (auto& pixel_coord : pixel_list) {
    auto act_x = pixel_coord.m_x - m_offset.m_x;
    auto act_y = pixel_coord.m_y - m_offset.m_y;

    if (act_x >= 0 && act_y >= 0 && act_x < width && act_y < height) {
      m_neighbour_image->setValue(act_x, act_y, -1);
    }
  }

  for (int act_y = 0; act_y < height; ++act_y) {
    for (int act_x = 0; act_x < width; ++act_x) {
      int offset_x = act_x + m_offset.m_x;
      int offset_y = act_y + m_offset.m_y;

      // set surrounding pixels that do not belong to the image and are above the threshold to 1, all others to 0
      if (offset_x >= 0 && offset_y >= 0 && offset_x < threshold_image->getWidth() &&
          offset_y < threshold_image->getHeight()) {
        if (threshold_image->getValue(offset_x, offset_y) > 0) {
          if (m_neighbour_image->getValue(act_x, act_y) != -1) {
            m_neighbour_image->setValue(act_x, act_y, 1);
          }
          else {
            m_neighbour_image->setValue(act_x, act_y, 0);
          }
        }
        else {
          m_neighbour_image->setValue(act_x, act_y, 0);
        }
      }
    }
  }
}

bool NeighbourInfo::isNeighbourObjectPixel(int x, int y) const {
  int act_x = x - m_offset.m_x;
  int act_y = y - m_offset.m_y;
  assert(act_x >= 0 && act_y >= 0 && act_x < m_neighbour_image->getWidth() && act_y < m_neighbour_image->getHeight());
  return m_neighbour_image->getValue(act_x, act_y);
}

} // end SExtractor
