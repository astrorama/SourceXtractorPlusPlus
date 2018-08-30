/*
 * PointModelFitting.h
 *
 *  Created on: July 11, 2018
 *      Author: Alejandro Álvarez
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTING_H_
#define _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTING_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class PointModelFitting: public Property {
public:
  PointModelFitting(
    SeFloat x, SeFloat y, SeFloat wc_alpha, SeFloat wc_delta,
    SeFloat total_flux, unsigned int iterations):
  m_x(x), m_y(y),
  m_wc_alpha(wc_alpha), m_wc_delta(wc_delta),
  m_total_flux(total_flux), m_iterations(iterations)
  {

  }

  SeFloat getX() const {
    return m_x;
  }

  SeFloat getY() const {
    return m_y;
  }

  SeFloat getAlpha() const {
    return m_wc_alpha;
  }

  SeFloat getDelta() const {
    return m_wc_delta;
  }

  SeFloat getTotalFlux() const {
    return m_total_flux;
  }

  unsigned getIterations() const {
    return m_iterations;
  }

private:
  SeFloat m_x, m_y;
  SeFloat m_wc_alpha, m_wc_delta; // world coordinate position

  SeFloat m_total_flux;

  unsigned int m_iterations;
};

} // SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTING_H_ */
