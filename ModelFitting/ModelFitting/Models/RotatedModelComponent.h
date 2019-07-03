/** 
 * @file RotatedModelComponent.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_ROTATEDMODELCOMPONENT_H
#define	MODELFITTING_ROTATEDMODELCOMPONENT_H

#include <memory> // for std::unique_ptr
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

/**
 * Decorates a model component, rotating the coordinates by the given angle.
 * As there is no scaling, the integrated value remains.
 */
class RotatedModelComponent : public ModelComponent {
  
public:

  /**
   * Constructor
   * @param component
   *    The component to be decorated. The ownership is acquired by the new object.
   * @param rotation_angle
   *    The rotation angle in radians.
   */
  RotatedModelComponent(std::unique_ptr<ModelComponent> component,
                        BasicParameter& rotation_angle);

  RotatedModelComponent(RotatedModelComponent&& other);
            
  virtual ~RotatedModelComponent();
          
  double getValue(double x, double y) override;
  
  void updateRasterizationInfo(double scale, double r_max) override;
  
  std::vector<ModelSample> getSharpSampling() override;
  
  bool insideSharpRegion(double x, double y) override;
  
private:
  
  std::unique_ptr<ModelComponent> m_component;
  double m_rotation_angle;
  double m_cos;
  double m_sin;
  ReferenceUpdater m_rotation_angle_updater;
  
}; // end of class RotatedModelComponent

} // end of namespace ModelFitting

#endif	/* MODELFITTING_ROTATEDMODELCOMPONENT_H */

