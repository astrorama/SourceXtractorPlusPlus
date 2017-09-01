/*
 * CoordinateSystem.h
 *
 *  Created on: Nov 17, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_COORDINATESYSTEM_COORDINATESYSTEM_H_
#define _SEFRAMEWORK_COORDINATESYSTEM_COORDINATESYSTEM_H_

#include "SEUtils/PixelCoordinate.h"

namespace SExtractor {

struct WorldCoordinate {
  double m_alpha, m_delta;

  WorldCoordinate() : m_alpha(0), m_delta(0) {}
  WorldCoordinate(double alpha, double delta) : m_alpha(alpha), m_delta(delta) {}
};

struct ImageCoordinate {
  double m_x, m_y;

  ImageCoordinate() : m_x(0), m_y(0) {}
  ImageCoordinate(double x, double y) : m_x(x), m_y(y) {}
};


class CoordinateSystem {
public:
  virtual ~CoordinateSystem() = default;

  virtual WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const = 0;
  virtual ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const = 0;
};

}

#endif /* _SEFRAMEWORK_COORDINATESYSTEM_COORDINATESYSTEM_H_ */
