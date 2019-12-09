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
                std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale,
                std::shared_ptr<BasicParameter> rotation_angle, double width, double height,
                std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y);
  
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

