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
