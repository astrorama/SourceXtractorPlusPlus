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
 * @file OldSharp.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Models/OldSharp.h"

namespace ModelFitting {

OldSharp::OldSharp(double r_sharp_pix, double log_incr, double first_r)
        : m_r_sharp_pix {r_sharp_pix}, m_log_incr{log_incr}, m_first_r{first_r} {
}

OldSharp::~OldSharp() = default;

void OldSharp::updateRasterizationInfo(double scale, double, Profile) {
  m_r_sharp = m_r_sharp_pix * scale;
}

bool OldSharp::insideSharpRegion(double r) {
  return r < m_r_sharp;
}

std::pair<double, int> OldSharp::nextRadiusAndAngleNo(double prev_r) {
  double next_r = prev_r > 0
                  ? prev_r * m_log_incr
                  : m_first_r;
  return std::make_pair(next_r, 72);
}

} // end of namespace ModelFitting
