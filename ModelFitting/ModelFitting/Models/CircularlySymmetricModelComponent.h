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
 * @file CircularlySymmetricModelComponent.h
 * @date August 25, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_CIRCULARLYSYMMETRICMODELCOMPONENT_H
#define	MODELFITTING_CIRCULARLYSYMMETRICMODELCOMPONENT_H

#include <memory> // For std::unique_ptr
#include <vector>
#include "ModelFitting/Models/SharpRegionManager.h"
#include "ModelFitting/Models/ModelComponent.h"
#include "ModelFitting/Models/SersicProfile.h"

namespace ModelFitting {

/**
 * A model component that uses a 1D function - or profile - to build a circular and symmetric 2D model.
 * @see SersicProfile as an example of such a profile
 */
template <typename Profile>
class CircularlySymmetricModelComponent : public ModelComponent {
  
public:

  /**
   * Constructor
   * @param sharp_manager
   *    The model component delegates to this region manager the determination of the sharp area.
   *    Note that you may build two different components with the same profile but different strategies
   *    for the sharp region.
   * @param proj_parameters
   *    Parameters that are forwarded to the profile
   */
  template <typename... ProfileParameters>
  CircularlySymmetricModelComponent(std::unique_ptr<SharpRegionManager> sharp_manager,
                                    ProfileParameters&&... proj_parameters);

  virtual ~CircularlySymmetricModelComponent();
  
  virtual double getValue(double x, double y);
  
  virtual void updateRasterizationInfo(double scale, double r_max);

  virtual std::vector<ModelSample> getSharpSampling();
  
  virtual bool insideSharpRegion(double x, double y);
  
private:
  
  std::unique_ptr<SharpRegionManager> m_sharp_manager;
  Profile m_profile;
  
}; // end of class CircularlySymmetricModelComponent

using SersicModelComponent = CircularlySymmetricModelComponent<SersicProfile>;

} // end of namespace ModelFitting

#include "_impl/CircularlySymmetricModelComponent.icpp"

#endif	/* MODELFITTING_CIRCULARLYSYMMETRICMODELCOMPONENT_H */

