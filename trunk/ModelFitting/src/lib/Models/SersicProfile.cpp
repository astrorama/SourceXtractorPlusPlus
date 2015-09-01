/** 
 * @file SersicProfile.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include <cmath>
#include <memory>
#include "ModelFitting/Models/SersicProfile.h"

namespace ModelFitting {

SersicProfile::SersicProfile(BasicParameter& i0, BasicParameter& n, BasicParameter& k)
        : m_i0 {i0.getValue()}, m_n{n.getValue()}, m_k{k.getValue()},
          m_i0_updater{i0, m_i0}, m_n_updater{n, m_n}, m_k_updater{k, m_k} {
}

SersicProfile::SersicProfile(const SersicProfile& other)
        : m_i0 {other.m_i0}, m_n{other.m_n}, m_k{other.m_k},
          m_i0_updater{other.m_i0_updater.getParameter(), m_i0},
          m_n_updater{other.m_n_updater.getParameter(), m_n},
          m_k_updater{other.m_k_updater.getParameter(), m_k} {
}

SersicProfile::~SersicProfile() = default;

double SersicProfile::operator()(double r) const {
  return m_i0 * std::exp(-m_k * std::pow(r, 1. / m_n));
}

} // end of namespace ModelFitting