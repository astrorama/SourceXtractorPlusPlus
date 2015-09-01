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

class AutoSharp : public SharpRegionManager {
  
public:
  
  AutoSharp(double log_incr=1.122, double first_r=1E-4, double tolerance=0.05,
            double max_step_factor=5.);
  
  virtual ~AutoSharp();
  
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
  double m_max_step_factor;
  
}; // end of class AutoSharp

} // end of namespace ModelFitting

#endif	/* MODELFITTING_AUTOSHARP_H */

