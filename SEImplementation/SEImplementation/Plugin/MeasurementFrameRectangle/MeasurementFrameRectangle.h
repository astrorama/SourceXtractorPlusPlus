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

  MeasurementFrameRectangle(PixelCoordinate min_coord, PixelCoordinate max_coord):
    m_min_coord{min_coord}, m_max_coord(max_coord) {}

  PixelCoordinate getTopLeft() const {
    return m_min_coord;
  }

  PixelCoordinate getBottomRight() const {
    return m_max_coord;
  }

  int getWidth() const {
    return m_max_coord.m_x - m_min_coord.m_x;
  }

  int getHeight() const {
    return m_max_coord.m_y - m_min_coord.m_y;
  }

private:
  PixelCoordinate m_min_coord, m_max_coord;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLE_H_
