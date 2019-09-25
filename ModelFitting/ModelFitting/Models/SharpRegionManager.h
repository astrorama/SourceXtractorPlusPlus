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
 * @file SharpRegionManager.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_SHARPREGIONMANAGER_H
#define	MODELFITTING_SHARPREGIONMANAGER_H

#include <utility>
#include <functional>
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {


/**
 * Interface for sampling strategies.
 *
 * @description
 * For relatively flat portions of a function, just getting the value of the function at the center of a pixel and
 * multiplying by its area can be a good enough estimation, but for sharp "peaky" areas
 * this approach is not good, as we may be over or under estimating the integrated value for the area of that pixel.
 *
 * @note
 * This assumes a circular, symmetric profile
 */
class SharpRegionManager {
  
public:

  /**
   * Profile of the sampled function.
   */
  using Profile = std::function<double(double)>;

  /**
   * Destructor
   */
  virtual ~SharpRegionManager() = default;

  /**
   * Allows the region manager to update whatever internal information it needs to decide on the sharp
   * region that needs sampling.
   * @param scale
   *    Pixel scale
   * @param r_max
   *    Limit the sampling region to this value
   * @param profile
   *    Profile to be oversampled. It can be used to adjust to the actual function.
   */
  virtual void updateRasterizationInfo(double scale, double r_max,
                                       Profile profile) = 0;

  /**
   * @param r
   *    Distance to the origin of the function
   * @return
   *    True if the distance falls within the sharp region
   */
  virtual bool insideSharpRegion(double r) = 0;

  /**
   * Computes the next sampling distance, and how many points around the
   * circumference must be sampled at this distance.
   * @param prev_r
   *    Previous sampling distance. 0 for the first call.
   * @return
   *    A pair, where the first element is the distance from the center (radius), and the second
   *    is the number of sample points are to be taken: if, for instance, it were 4, then four samples
   *    must be taken at angles 0, pi/4, pi and pi*3/4
   */
  virtual std::pair<double, int> nextRadiusAndAngleNo(double prev_r) = 0;
  
}; // end of class SharpRegionManager

} // end of namespace ModelFitting

#endif	/* MODELFITTING_SHARPREGIONMANAGER_H */

