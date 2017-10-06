/*
 * MultiframeModelFitting.h
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTING_H_
#define _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTING_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class MultiframeModelFitting : public Property {
public:

  virtual ~MultiframeModelFitting() = default;

  MultiframeModelFitting(
      SeFloat x, SeFloat y,
      SeFloat wc_alpha, SeFloat wc_delta,
      SeFloat total_flux, unsigned int iterations) :
    m_x(x),
    m_y(y),
    m_wc_alpha(wc_alpha),
    m_wc_delta(wc_delta),
    m_total_flux(total_flux),
    m_iterations(iterations) {}

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

  SeFloat getIterations() const {
    return m_iterations;
  }

private:
  SeFloat m_x, m_y; // pixel coordinates on detection frame
  SeFloat m_wc_alpha, m_wc_delta; // world coordinate position

  SeFloat m_total_flux;
  unsigned int m_iterations;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTING_H_ */
