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

