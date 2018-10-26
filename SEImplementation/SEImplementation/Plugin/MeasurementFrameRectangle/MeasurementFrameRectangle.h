/*
 * MeasurementFrameRectangle.h
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLE_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEPRECTANGLE_MEASUREMENTFRAMERECTANGLE_H_


#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class MeasurementFrameRectangle: public Property {
public:
  virtual ~MeasurementFrameRectangle() = default;

  MeasurementFrameRectangle():
  m_min_coord{-1, -1}, m_max_coord{-1, -1} {}

  MeasurementFrameRectangle(PixelCoordinate min_coord, PixelCoordinate max_coord):
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

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLE_H_
