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
