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

class RotatedModelComponent : public ModelComponent {
  
public:
  
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

