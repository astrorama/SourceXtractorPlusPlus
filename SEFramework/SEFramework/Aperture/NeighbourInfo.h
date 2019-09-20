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
 * NeighbourInfo.h
 *
 *  Created on: Oct 12, 2018
 *      Author: Alejandro Alvarez
 */

#ifndef _SEFRAMEWORK_SEFRAMEWORK_APERTURE_NEIGHBOURINFO_H
#define _SEFRAMEWORK_SEFRAMEWORK_APERTURE_NEIGHBOURINFO_H

#include <vector>
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEUtils/PixelCoordinate.h"

namespace SExtractor {

class NeighbourInfo {
public:
  virtual ~NeighbourInfo() = default;

  NeighbourInfo(const PixelCoordinate &min_pixel, const PixelCoordinate &max_pixel,
                const std::vector<PixelCoordinate> &pixel_list,
                const std::shared_ptr<Image<SeFloat>> &threshold_image);


  bool isNeighbourObjectPixel(int x, int y) const;

private:
  std::shared_ptr<VectorImage<int>> m_neighbour_image;
  PixelCoordinate m_offset;
};

} // end SExtractor

#endif // _SEFRAMEWORK_SEFRAMEWORK_APERTURE_NEIGHBOURINFO_H
