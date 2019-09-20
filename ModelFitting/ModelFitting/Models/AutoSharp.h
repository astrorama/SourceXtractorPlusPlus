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
 * @file AutoSharp.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_AUTOSHARP_H
#define	MODELFITTING_AUTOSHARP_H

#include <utility> // For std::pair
#include "ModelFitting/Models/SharpRegionManager.h"

namespace ModelFitting {

/**
 * AutoSharp decides the sharp region evaluating the function in steps until the relative
 * error for the linear interpolation is below a given threshold
 */
class AutoSharp : public SharpRegionManager {
  
public:

  /**
   * Constructor
   * @param log_incr
   *    Increment multiplicative step
   * @param first_r
   *    First radius
   * @param tolerance
   *    The sharp region finishes when the relative error between the function and its linear interpolation
   *    is below this limit
   * @param min_sampling_factor
   *    Take at least this many samples
   */
  AutoSharp(double log_incr=1.122, double first_r=1E-4, double tolerance=0.05,
            double min_sampling_factor=5.);
  
  virtual ~AutoSharp();

  /**
   * @details
   *    The profile is used to find the sharp region, but without going further than
   *    r_max.
   */
  void updateRasterizationInfo(double scale, double r_max,
                               Profile profile) override;
  
  bool insideSharpRegion(double r) override;
  
  std::pair<double, int> nextRadiusAndAngleNo(double prev_r) override;
  
private:
  
  double m_r_sharp {0.};
  double m_max_step {0.};
  double m_first_pix_r {0.};
  double m_log_incr;
  double m_first_r;
  double m_tolerance;
  double m_min_sampling_factor;
  
}; // end of class AutoSharp

} // end of namespace ModelFitting

#endif	/* MODELFITTING_AUTOSHARP_H */

