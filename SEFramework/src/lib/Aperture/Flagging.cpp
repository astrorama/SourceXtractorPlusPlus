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



#include "SEFramework/Aperture/Flagging.h"

namespace SourceXtractor {

const SeFloat CROWD_THRESHOLD_APER = 0.1;
const SeFloat BADAREA_THRESHOLD_APER = 0.1;

Flags computeFlags(const std::shared_ptr<Aperture>& aperture,
                   SeFloat centroid_x, SeFloat centroid_y,
                   const std::vector<PixelCoordinate>& pix_list,
                   const std::shared_ptr<Image<SeFloat>>& detection_img,
                   const std::shared_ptr<Image<SeFloat>>& detection_variance,
                   const std::shared_ptr<Image<SeFloat>>& threshold_image,
                   SeFloat variance_threshold) {
  // get the aperture borders on the image
  auto min_pixel = aperture->getMinPixel(centroid_x, centroid_y);
  auto max_pixel = aperture->getMaxPixel(centroid_x, centroid_y);

  // get the neighbourhood information
  NeighbourInfo neighbour_info(min_pixel, max_pixel, pix_list, threshold_image);

  Flags flag = Flags::NONE;
  SeFloat total_area = 0.0;
  SeFloat bad_area = 0;
  SeFloat full_area = 0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {

      // get the area coverage and continue if there is overlap
      auto area = aperture->getArea(centroid_x, centroid_y, pixel_x, pixel_y);
      if (area == 0) {
        continue;
      }

      // make sure the pixel is inside the image
      if (pixel_x >= 0 && pixel_y >= 0 && pixel_x < detection_img->getWidth() && pixel_y < detection_img->getHeight()) {

        // enhance the area
        total_area += area;

        auto variance_tmp = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;

        full_area += neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y);
        bad_area += (variance_tmp > variance_threshold);
      }
      else {
        flag |= Flags::BOUNDARY;
      }
    }
  }

  // check/set the bad area flag
  if (total_area > 0 && bad_area / total_area > BADAREA_THRESHOLD_APER)
    flag |= Flags::BIASED;

  // check/set the crowded area flag
  if (total_area > 0 && full_area / total_area > CROWD_THRESHOLD_APER)
    flag |= Flags::NEIGHBORS;

  return flag;
}

} // end of namespace SourceXtractor
