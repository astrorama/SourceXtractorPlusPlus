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
/** 
 * @file AutoSharp.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include <cmath>  // For std::abs, std::ceil
#include <algorithm> // For std::min, std::max
#include "ModelFitting/Models/AutoSharp.h"

namespace ModelFitting {

AutoSharp::AutoSharp(double log_incr, double first_r, double tolerance, double min_sampling_factor)
          : m_log_incr {log_incr}, m_first_r{first_r}, m_tolerance{tolerance},
            m_min_sampling_factor{min_sampling_factor} {
}

AutoSharp::~AutoSharp() = default;

void AutoSharp::updateRasterizationInfo(double scale, double r_max, Profile profile) {
  m_r_sharp = 0.;
  double err = m_tolerance;
  while (m_r_sharp < r_max && err >= m_tolerance) {
    double v1 = profile(m_r_sharp);
    double v2 = profile(m_r_sharp + scale / 2.);
    double v3 = profile(m_r_sharp + scale);
    err = std::abs((v2 - (v3 + v1) / 2) / v2);
    m_r_sharp += scale;
  }
  m_r_sharp = std::min(m_r_sharp, r_max);
  m_r_sharp = std::max(m_r_sharp, 4 * scale);
  m_max_step = scale / m_min_sampling_factor;
  m_first_pix_r = scale / 2.;
}

bool AutoSharp::insideSharpRegion(double r) {
  return r < m_r_sharp;
}

std::pair<double, int> AutoSharp::nextRadiusAndAngleNo(double prev_r) {
  double next_r = prev_r > 0
                  ? std::min(prev_r * m_log_incr, prev_r + m_max_step)
                  : m_first_r;
  int angle_no = next_r > m_first_pix_r
                 ? std::ceil(2. * M_PI * prev_r / m_max_step)
                 : 1;
  return std::make_pair(next_r, angle_no);
}

} // end of namespace ModelFitting