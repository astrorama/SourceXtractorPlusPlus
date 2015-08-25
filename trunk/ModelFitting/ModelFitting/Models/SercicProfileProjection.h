/** 
 * @file SercicProfileProjection.h
 * @date August 25, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_SERSICPROFILEPROJECTION_H
#define	MODELFITTING_SERSICPROFILEPROJECTION_H

#include <cmath>
#include <memory>
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"

namespace ModelFitting {

class SersicProfileProjection {
  
public:
  
  SersicProfileProjection(BasicParameter& i0, BasicParameter& n, BasicParameter& k)
            : m_i0{i0.getValue()}, m_n{n.getValue()}, m_k{k.getValue()},
              m_i0_updater{i0, m_i0}, m_n_updater{n, m_n}, m_k_updater{k, m_k} {
  }
              
  SersicProfileProjection(const SersicProfileProjection& other)
            : m_i0{other.m_i0}, m_n{other.m_n}, m_k{other.m_k},
              m_i0_updater{other.m_i0_updater.getParameter(), m_i0},
              m_n_updater{other.m_n_updater.getParameter(), m_n},
              m_k_updater{other.m_k_updater.getParameter(), m_k} { }
  
  virtual ~SersicProfileProjection() = default;
  
  double operator()(double r) const {
    return m_i0 * std::exp(-m_k * std::pow(r, 1./m_n));
  }
  
private:
  
  double m_i0;
  double m_n;
  double m_k;
  ReferenceUpdater m_i0_updater;
  ReferenceUpdater m_n_updater;
  ReferenceUpdater m_k_updater;
  
}; // end of class SersicProfileProjection

} // end of namespace ModelFitting

#endif	/* MODELFITTING_SERSICPROFILEPROJECTION_H */

