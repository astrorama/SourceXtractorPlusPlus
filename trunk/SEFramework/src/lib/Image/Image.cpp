/**
 * @file src/lib/Image/Image.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageBuffer.h"

using SEUtils::PixelCoordinate;

namespace SEFramework {

std::shared_ptr<Image> Image::getSubImage(PixelCoordinate min_coord, PixelCoordinate max_coord) const {
  auto width = max_coord.m_x - min_coord.m_x + 1;
  auto height = max_coord.m_y - min_coord.m_y + 1;

  auto sub_image = std::make_shared<ImageBuffer>(width, height);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
       sub_image->setValue(x ,y, getValue(x + min_coord.m_x, y + min_coord.m_y));
    }
  }

  return sub_image;
}



} // SEFramework namespace



