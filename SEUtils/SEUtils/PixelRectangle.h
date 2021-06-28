/*
 * PixelRectangle.h
 *
 *  Created on: Jun 9, 2021
 *      Author: mschefer
 */

#ifndef _SEUTILS_PIXELRECTANGLE_H_
#define _SEUTILS_PIXELRECTANGLE_H_

#include "SEUtils/PixelCoordinate.h"

namespace SourceXtractor {

class PixelRectangle {
public:

  PixelRectangle():
    m_min_coord{-1, -1}, m_max_coord{-1, -1} {}

  PixelRectangle(const PixelRectangle& rectangle) :
    m_min_coord{rectangle.m_min_coord}, m_max_coord{rectangle.m_max_coord} {}

  PixelRectangle(PixelCoordinate min_coord, PixelCoordinate max_coord):
    m_min_coord{min_coord}, m_max_coord{max_coord} {
    assert(min_coord.m_x <= max_coord.m_x && min_coord.m_y <= max_coord.m_y);
  }

  PixelCoordinate getTopLeft() const {
    assert(m_max_coord.m_x >= 0);
    return m_min_coord;
  }

  PixelCoordinate getBottomRight() const {
    assert(m_max_coord.m_x >= 0);
    return m_max_coord;
  }

  int getWidth() const {
    if (m_max_coord.m_x < 0)
      return 0;
    return m_max_coord.m_x - m_min_coord.m_x + 1;
  }

  int getHeight() const {
    if (m_max_coord.m_x < 0)
      return 0;
    return m_max_coord.m_y - m_min_coord.m_y + 1;
  }

private:
  PixelCoordinate m_min_coord, m_max_coord;
};

}

#endif /* _SEUTILS_PIXELRECTANGLE_H_ */
