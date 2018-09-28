/*
 * NeighbourInfo.h
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFO_H_
#define _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFO_H_

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Property/Property.h"


namespace SExtractor {

class NeighbourInfo : public Property {
public:
  /// Destructor
  virtual ~NeighbourInfo() = default;

  NeighbourInfo() : m_neighbour_image(nullptr) {
  }

  NeighbourInfo(PixelCoordinate offset, const std::shared_ptr<Image < int>>

  &neighbour_image):
  m_offset{ offset }, m_neighbour_image{ neighbour_image } {
  }

  /// Checks
  int isNeighbourObjectPixel(int x, int y) const {
    assert(m_neighbour_image);
    x -= m_offset.m_x;
    y -= m_offset.m_y;
    if (x < 0 || y < 0 || x >= m_neighbour_image->getWidth() || y >= m_neighbour_image->getHeight())
      return 0;
    return m_neighbour_image->getValue(x, y);
  }

  std::shared_ptr<const Image<int>> getImage() const {
    return m_neighbour_image;
  }

private:
  std::shared_ptr<Image < int>> m_neighbour_image;
  PixelCoordinate m_offset;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFO_H_
