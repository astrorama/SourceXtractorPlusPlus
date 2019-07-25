/** 
 * @file ExtendedModel.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_EXTENDEDMODEL_H
#define	MODELFITTING_EXTENDEDMODEL_H

#include <vector>
#include <memory> // for std::unique_ptr
#include <numeric> // for std::accumulate

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Models/PositionedModel.h"
#include "ModelFitting/Models/ModelComponent.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/utils.h"

namespace ModelFitting {

template <typename ImageType>
class ExtendedModel : public PositionedModel {
  
public:
  
  ExtendedModel(std::vector<std::unique_ptr<ModelComponent>>&& component_list,
                BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation_angle, double width, double height,
                BasicParameter& x, BasicParameter& y);
  
  ExtendedModel(ExtendedModel&&) = default;
  
  virtual ~ExtendedModel() = default;
  
  virtual double getValue(double x, double y) const;
  
  virtual ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const;
  
  double getWidth() const {
    return m_width;
  }

  double getHeight() const {
    return m_height;
  }

protected:
  std::vector<std::unique_ptr<ModelComponent>> m_component_list {};

private:
  double m_width;
  double m_height;
  
}; // end of class ExtendedModel
  
} // end of namespace ModelFitting

#include "_impl/ExtendedModel.icpp"

#endif	/* MODELFITTING_EXTENDEDMODEL_H */

