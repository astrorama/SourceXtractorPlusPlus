/** 
 * @file ScaledModelComponent.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_SCALEDMODELCOMPONENT_H
#define	MODELFITTING_SCALEDMODELCOMPONENT_H

#include <memory> // for std::unique_ptr
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

/**
 * Decorates a model component, scaling the *axes* by the given parameters.
 * @details
 *  For instance, for a model scaled by 2 in each axes, a call to getValue(1,1) is
 *  equivalent to a getValue(0.5, 0.5) to the underlying function.
 *
 * @note
 *  The integral value is not kept, but rather increased approximately by (x_scale * y_scale)
 */
class ScaledModelComponent : public ModelComponent {
  
public:

  /**
   * Constructor
   * @param component
   *    The component to be decorated. The ownership is acquired by the new object.
   * @param x_scale
   *    Scale for the X axis
   * @param y_scale
   *    Scale for the Y axis
   */
  ScaledModelComponent(std::unique_ptr<ModelComponent> component,
                       BasicParameter& x_scale, BasicParameter& y_scale);

  ScaledModelComponent(ScaledModelComponent&& other);
            
  virtual ~ScaledModelComponent();
          
  double getValue(double x, double y) override;
  
  void updateRasterizationInfo(double scale, double r_max) override;
  
  std::vector<ModelSample> getSharpSampling() override;
  
  bool insideSharpRegion(double x, double y) override;
  
private:
  
  std::unique_ptr<ModelComponent> m_component;
  double m_x_scale;
  double m_y_scale;
  ReferenceUpdater m_x_scale_updater;
  ReferenceUpdater m_y_scale_updater;
  
}; // end of class ScaledModelComponent

} // end of namespace ModelFitting

#endif	/* MODELFITTING_SCALEDMODELCOMPONENT_H */

