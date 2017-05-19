/*
 * ShapeParameters.h
 *
 *  Created on: Jan 27, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERS_H_
#define _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERS_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class ShapeParameters : public Property {
public:

  ShapeParameters(SeFloat a, SeFloat b, SeFloat theta, SeFloat abcor, SeFloat cxx, SeFloat cyy, SeFloat cxy, SeFloat area)
    : m_a(a), m_b(b), m_theta(theta), m_abcor(abcor), m_cxx(cxx), m_cyy(cyy), m_cxy(cxy), m_area(area) {}

  virtual ~ShapeParameters() = default;

  SeFloat getEllipseA() const {
    return m_a;
  }

  SeFloat getEllipseB() const {
    return m_b;
  }

  SeFloat getEllipseTheta() const {
    return m_theta;
  }

  SeFloat getEllipseCxx() const {
    return m_cxx;
  }

  SeFloat getEllipseCyy() const {
    return m_cyy;
  }

  SeFloat getEllipseCxy() const {
    return m_cxy;
  }

  SeFloat getAbcor() const {
    return m_abcor;
  }

  SeFloat getArea() const {
    return m_area;
  }

private:
  SeFloat m_a, m_b, m_theta; // Ellipse semi-major axis, semi-minor axis, and angle
  SeFloat m_abcor;
  SeFloat m_cxx, m_cyy, m_cxy;
  SeFloat m_area;

};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERS_H_ */
