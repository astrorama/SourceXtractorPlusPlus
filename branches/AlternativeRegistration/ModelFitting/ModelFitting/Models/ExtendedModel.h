/** 
 * @file ExtendedModel.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_EXTENDEDMODEL_H
#define	MODELFITTING_EXTENDEDMODEL_H

#include <vector>
#include <memory> // for std::unique_ptr
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Models/PositionedModel.h"
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

class ExtendedModel : public PositionedModel {
  
public:
  
  ExtendedModel(std::vector<std::unique_ptr<ModelComponent>> component_list,
                BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation_angle, double width, double height,
                BasicParameter& x, BasicParameter& y);
  
  ExtendedModel(ExtendedModel&&);
  
  virtual ~ExtendedModel();
  
  double getValue(double x, double y) const;
  
  template <typename ImageType>
  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const;
  
  double getWidth() const;
  
  double getHeight() const;
  
private:
  
  std::vector<std::unique_ptr<ModelComponent>> m_component_list {};
  double m_width;
  double m_height;
  
}; // end of class ExtendedModel
  
} // end of namespace ModelFitting

#include "_impl/ExtendedModel.icpp"

#endif	/* MODELFITTING_EXTENDEDMODEL_H */

