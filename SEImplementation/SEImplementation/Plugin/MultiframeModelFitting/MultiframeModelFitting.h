/*
 * MultiframeModelFitting.h
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTING_H_
#define _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTING_H_

#include <vector>

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class MultiframeModelFitting : public Property {
public:

  virtual ~MultiframeModelFitting() = default;

  MultiframeModelFitting(
      SeFloat x, SeFloat y,
      SeFloat wc_alpha, SeFloat wc_delta,
      SeFloat exp_radius, SeFloat dev_radius,
      std::vector<double> fluxes, std::vector<double> exp_fluxes, std::vector<double> dev_fluxes,
      unsigned int iterations, SeFloat chi_squared) :
    m_x(x),
    m_y(y),
    m_wc_alpha(wc_alpha),
    m_wc_delta(wc_delta),
    m_exp_radius(exp_radius),
    m_dev_radius(dev_radius),
    m_fluxes(fluxes),
    m_exp_fluxes(exp_fluxes),
    m_dev_fluxes(dev_fluxes),
    m_iterations(iterations),
    m_chi_squared(chi_squared) {}

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

  SeFloat getExpRadius() const {
    return m_exp_radius;
  }

  SeFloat getDevRadius() const {
    return m_dev_radius;
  }

//  SeFloat getTotalFlux() const {
//    return m_total_flux;
//  }

  std::vector<double> getFluxes() const {
    return m_fluxes;
  }

  std::vector<double> getExpFluxes() const {
    return m_exp_fluxes;
  }

  std::vector<double> getDevFluxes() const {
    return m_dev_fluxes;
  }

  unsigned int getIterations() const {
    return m_iterations;
  }

  SeFloat getReducedChiSquared() const {
    return m_chi_squared;
  }

private:
  SeFloat m_x, m_y;
  SeFloat m_wc_alpha, m_wc_delta; // world coordinate position
  SeFloat m_exp_radius, m_dev_radius;

  std::vector<double> m_fluxes, m_exp_fluxes, m_dev_fluxes;
  unsigned int m_iterations;
  SeFloat m_chi_squared;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTING_H_ */
