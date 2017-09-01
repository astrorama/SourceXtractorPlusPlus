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

class ScaledModelComponent : public ModelComponent {
  
public:

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

