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

namespace SourceXtractor {

NeighbourInfo::NeighbourInfo(const PixelCoordinate& min_pixel, const PixelCoordinate& max_pixel,
                             const std::vector<SourceXtractor::PixelCoordinate>& pixel_list,
                             const std::shared_ptr<SourceXtractor::Image<SourceXtractor::SeFloat>>& threshold_image)
  : m_offset{min_pixel} {
  m_offset.clip(threshold_image->getWidth(), threshold_image->getHeight());

  auto max_pixel_copy = max_pixel;
  max_pixel_copy.clip(threshold_image->getWidth(), threshold_image->getHeight());

  auto width = max_pixel_copy.m_x - m_offset.m_x + 1;
  auto height = max_pixel_copy.m_y - m_offset.m_y + 1;

  m_neighbour_image = VectorImage<int>::create(width, height);

  auto threshold_cutout = threshold_image->getChunk(m_offset, max_pixel_copy);

  for (auto& pixel_coord : pixel_list) {
    auto act_x = pixel_coord.m_x - m_offset.m_x;
    auto act_y = pixel_coord.m_y - m_offset.m_y;

    if (act_x >= 0 && act_y >= 0 && act_x < width && act_y < height) {
      m_neighbour_image->setValue(act_x, act_y, -1);
    }
  }

  for (int act_y = 0; act_y < height; ++act_y) {
    for (int act_x = 0; act_x < width; ++act_x) {
      // set surrounding pixels that do not belong to the image and are above the threshold to 1,
      // all others to 0
      bool is_above_threshold = threshold_cutout->getValue(act_x, act_y) > 0;
      bool belongs = m_neighbour_image->getValue(act_x, act_y) == -1;
      m_neighbour_image->setValue(act_x, act_y, is_above_threshold && !belongs);
    }
  }
}

bool NeighbourInfo::isNeighbourObjectPixel(int x, int y) const {
  int act_x = x - m_offset.m_x;
  int act_y = y - m_offset.m_y;
  assert(m_neighbour_image->isInside(act_x, act_y));
  return m_neighbour_image->getValue(act_x, act_y);
}

} // end SourceXtractor
