/*
 * SimpleModelFitting.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTING_H_
#define _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTING_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class SimpleModelFitting : public Property {
public:

  virtual ~SimpleModelFitting() = default;

  SimpleModelFitting(
      SeFloat x, SeFloat y, SeFloat wc_alpha, SeFloat wc_delta,
      SeFloat total_flux, unsigned int iterations,
      SeFloat exp_ratio, SeFloat exp_angle,
      SeFloat dev_ratio, SeFloat dev_angle ) :
    m_x(x),
    m_y(y),
    m_wc_alpha(wc_alpha),
    m_wc_delta(wc_delta),
    m_total_flux(total_flux),
    m_iterations(iterations),
    m_exp_ratio(exp_ratio),
    m_exp_angle(exp_angle),
    m_dev_ratio(dev_ratio),
    m_dev_angle(dev_angle) {}

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

  // exponential model parameters
  SeFloat getExponentialRatio() const {
    return m_exp_angle;
  }

  SeFloat getExponentialAngle() const {
    return m_exp_angle;
  }

  // devaucouleurs model parameters
  SeFloat getDevaucouleursRatio() const {
    return m_exp_angle;
  }

  SeFloat getDevaucouleursAngle() const {
    return m_exp_angle;
  }

private:
  SeFloat m_x, m_y;
  SeFloat m_wc_alpha, m_wc_delta; // world coordinate position

  SeFloat m_total_flux;
  unsigned int m_iterations;

  // exponential model parameters
  SeFloat m_exp_ratio;
  SeFloat m_exp_angle;

  // devaucouleurs model parameters
  SeFloat m_dev_ratio;
  SeFloat m_dev_angle;
};

}




#endif /* _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTING_H_ */
