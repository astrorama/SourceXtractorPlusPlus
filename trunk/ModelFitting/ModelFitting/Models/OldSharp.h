/** 
 * @file OldSharp.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_OLDSHARP_H
#define	MODELFITTING_OLDSHARP_H

#include "ModelFitting/Models/SharpRegionManager.h"

namespace ModelFitting {

class OldSharp : public SharpRegionManager {
  
public:
  
  OldSharp(double r_sharp_pix=4., double log_incr=1.122, double first_r=1E-4);
  
  virtual ~OldSharp();
  
  void updateRasterizationInfo(double scale, double, ProfileProjection) override;
  
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

