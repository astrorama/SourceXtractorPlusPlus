/** 
 * @file CircularlySymmetricModelComponent.h
 * @date August 25, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_CIRCULARLYSYMMETRICMODELCOMPONENT_H
#define	MODELFITTING_CIRCULARLYSYMMETRICMODELCOMPONENT_H

#include <memory> // For std::unique_ptr
#include <vector>
#include "ModelFitting/Models/ModelComponent.h"
#include "ModelFitting/Models/SersicProfile.h"

namespace ModelFitting {

template <typename Profile>
class CircularlySymmetricModelComponent : public ModelComponent {
  
public:
  
  template <typename... ProfileParameters>
  CircularlySymmetricModelComponent(ProfileParameters&&... proj_parameters);
  
  virtual ~CircularlySymmetricModelComponent();
  
  virtual double getValue(double x, double y);
  
private:
  
  Profile m_profile;
  
}; // end of class CircularlySymmetricModelComponent

using SersicModelComponent = CircularlySymmetricModelComponent<SersicProfile>;

} // end of namespace ModelFitting

#include "_impl/CircularlySymmetricModelComponent.icpp"

#endif	/* MODELFITTING_CIRCULARLYSYMMETRICMODELCOMPONENT_H */

