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

class SersicProfile {
  
public:
  
  SersicProfile(BasicParameter& i0, BasicParameter& n, BasicParameter& k);
              
  SersicProfile(const SersicProfile& other);
  
  virtual ~SersicProfile();
  
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

