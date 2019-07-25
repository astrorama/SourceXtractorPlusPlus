/** 
 * @file SersicProfile.h
 * @date August 25, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_SERSICPROFILEPROJECTION_H
#define	MODELFITTING_SERSICPROFILEPROJECTION_H

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"

namespace ModelFitting {

/**
 * Mathematical function that describes how the intensity I of a galaxy varies with distance R.
 *
 * \f$ I=I_0 * e ^ {-k * R^{1 / n}} \f$
 */
class SersicProfile {
  
public:

  /**
   * Constructor
   * @param i0
   *    Intensity at the center
   * @param n
   *    Sersic index. n=1 is exponential and n=4 de Vaucouleurs
   * @param k
   *    A constant that multiplies the term \f$ R^{1 / n} \f$
   */
  SersicProfile(BasicParameter& i0, BasicParameter& n, BasicParameter& k);

  /**
   * Copy constructor
   */
  SersicProfile(const SersicProfile& other);

  /**
   * Destructor
   */
  virtual ~SersicProfile();

  /**
   * Evaluate the Sersic profile at the given distance
   * @param r
   *    The distance from the origin
   */
  double operator()(double r) const;
  
private:
  
  double m_i0;
  double m_n;
  double m_k;
  ReferenceUpdater m_i0_updater;
  ReferenceUpdater m_n_updater;
  ReferenceUpdater m_k_updater;
  
}; // end of class SersicProfile

} // end of namespace ModelFitting

#endif	/* MODELFITTING_SERSICPROFILEPROJECTION_H */

