/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

namespace SourceXtractor {

class MultiframeModelFitting : public Property {
public:

  virtual ~MultiframeModelFitting() = default;

  MultiframeModelFitting(
      SeFloat x, SeFloat y,
      SeFloat wc_alpha, SeFloat wc_delta,
      SeFloat exp_radius, SeFloat dev_radius,
      std::vector<double> fluxes, std::vector<double> exp_fluxes, std::vector<double> dev_fluxes,
      unsigned int iterations, SeFloat chi_squared, unsigned int nframes) :
    m_x(x),
    m_y(y),
    m_wc_alpha(wc_alpha),
    m_wc_delta(wc_delta),
    m_exp_radius(exp_radius),
    m_dev_radius(dev_radius),
    m_fluxes(fluxes),
    m_exp_fluxes(exp_fluxes),
    m_dev_fluxes(dev_fluxes),
    m_iterations(iterations), m_frame_count(nframes),
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

  unsigned int getFrameCount() const {
    return m_frame_count;
  }

  SeFloat getReducedChiSquared() const {
    return m_chi_squared;
  }

private:
  SeFloat m_x, m_y;
  SeFloat m_wc_alpha, m_wc_delta; // world coordinate position
  SeFloat m_exp_radius, m_dev_radius;

  std::vector<double> m_fluxes, m_exp_fluxes, m_dev_fluxes;
  unsigned int m_iterations, m_frame_count;
  SeFloat m_chi_squared;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTING_H_ */
