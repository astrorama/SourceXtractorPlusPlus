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

class SharpRegionManager {
  
public:
  
  using ProfileProjection = std::function<double(double)>;
  
  virtual ~SharpRegionManager() = default;

  virtual void updateRasterizationInfo(double scale, double r_max,
                                       ProfileProjection projection) = 0;
  
  virtual bool insideSharpRegion(double r) = 0;
  
  virtual std::pair<double, int> nextRadiusAndAngleNo(double prev_r) = 0;
  
}; // end of class SharpRegionManager

} // end of namespace ModelFitting

#endif	/* MODELFITTING_SHARPREGIONMANAGER_H */

