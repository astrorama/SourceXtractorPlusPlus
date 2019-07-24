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

