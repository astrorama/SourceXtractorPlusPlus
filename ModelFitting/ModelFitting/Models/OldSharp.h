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
 * @file OldSharp.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_OLDSHARP_H
#define	MODELFITTING_OLDSHARP_H

#include "ModelFitting/Models/SharpRegionManager.h"

namespace ModelFitting {

/**
 * Legacy implementation of the sharp region manager, matching the logic from SExtractor 2.
 *
 * @details
 * The number of samples at a given distance is always 72, and the sharp region is decided at
 * construction: the first sampling distance is first_r, and each step is incremented multiplying by  by log_incr.
 * The sharp region is determined by the distance r_sharp_pix, and taking into account the pixel scale:
 *  scale * r_sharp_pix
 */
class OldSharp : public SharpRegionManager {
  
public:

  /**
   * Constructor
   * @param r_sharp_pix
   *    Sharp region
   * @param log_incr
   *    Increment multiplicative step
   * @param first_r
   *    First radius
   */
  OldSharp(double r_sharp_pix=4., double log_incr=1.122, double first_r=1E-4);

  virtual ~OldSharp();

  /**
   * @note
   *    Only the scale parameter is used
   */
  void updateRasterizationInfo(double scale, double, Profile) override;

  bool insideSharpRegion(double r) override;

  std::pair<double, int> nextRadiusAndAngleNo(double prev_r) override;
  
private:
  
  double m_r_sharp_pix;
  double m_r_sharp {0.};
  double m_log_incr;
  double m_first_r;
  
}; // end of class OldSharp

} // end of namespace ModelFitting

#endif	/* MODELFITTING_OLDSHARP_H */

