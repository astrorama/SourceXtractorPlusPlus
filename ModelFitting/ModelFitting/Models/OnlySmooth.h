/** 
 * @file OnlySmooth.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_ONLYSMOOTH_H
#define	MODELFITTING_ONLYSMOOTH_H

#include "ModelFitting/Models/SharpRegionManager.h"

namespace ModelFitting {

class OnlySmooth : public SharpRegionManager {
  
public:
  
  virtual ~OnlySmooth();
  
  void updateRasterizationInfo(double, double, ProfileProjection) override;
  
  bool insideSharpRegion(double) override;
  
  std::pair<double, int> nextRadiusAndAngleNo(double) override;
  
}; // end of class OnlySmooth

} // end of namespace ModelFitting

#endif	/* MODELFITTING_ONLYSMOOTH_H */

