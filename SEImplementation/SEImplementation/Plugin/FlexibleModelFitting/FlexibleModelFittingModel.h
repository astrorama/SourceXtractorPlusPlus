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
/*
 * FlexibleModelFittingModel.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_

#include <vector>

#include "ModelFitting/Models/ConstantModel.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"

#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"

namespace SourceXtractor {

class FlexibleModelFittingParameterManager;

class FlexibleModelFittingModel {
public:
  virtual ~FlexibleModelFittingModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> reference_coordinates,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const = 0;

private:
};

class FlexibleModelFittingPointModel : public FlexibleModelFittingModel {
public:
  FlexibleModelFittingPointModel(std::shared_ptr<FlexibleModelFittingParameter> x,
                                 std::shared_ptr<FlexibleModelFittingParameter> y,
                                 std::shared_ptr<FlexibleModelFittingParameter> flux)
      : m_x(x), m_y(y), m_flux(flux) {}

  virtual ~FlexibleModelFittingPointModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> reference_coordinates,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;
private:
  std::shared_ptr<FlexibleModelFittingParameter> m_x;
  std::shared_ptr<FlexibleModelFittingParameter> m_y;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
};

class FlexibleModelFittingExponentialModel : public FlexibleModelFittingModel {
public:
  FlexibleModelFittingExponentialModel(std::shared_ptr<FlexibleModelFittingParameter> x,
                                  std::shared_ptr<FlexibleModelFittingParameter> y,
                                  std::shared_ptr<FlexibleModelFittingParameter> flux,
                                  std::shared_ptr<FlexibleModelFittingParameter> effective_radius,
                                  std::shared_ptr<FlexibleModelFittingParameter> aspect_ratio,
                                  std::shared_ptr<FlexibleModelFittingParameter> angle)
      : m_x(x),
        m_y(y),
        m_flux(flux),
        m_effective_radius(effective_radius),
        m_aspect_ratio(aspect_ratio),
        m_angle(angle) {}

  virtual ~FlexibleModelFittingExponentialModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> reference_coordinates,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_x;
  std::shared_ptr<FlexibleModelFittingParameter> m_y;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
  std::shared_ptr<FlexibleModelFittingParameter> m_effective_radius;
  std::shared_ptr<FlexibleModelFittingParameter> m_aspect_ratio;
  std::shared_ptr<FlexibleModelFittingParameter> m_angle;
};

class FlexibleModelFittingDevaucouleursModel : public FlexibleModelFittingModel {
public:
  FlexibleModelFittingDevaucouleursModel(std::shared_ptr<FlexibleModelFittingParameter> x,
                                  std::shared_ptr<FlexibleModelFittingParameter> y,
                                  std::shared_ptr<FlexibleModelFittingParameter> flux,
                                  std::shared_ptr<FlexibleModelFittingParameter> effective_radius,
                                  std::shared_ptr<FlexibleModelFittingParameter> aspect_ratio,
                                  std::shared_ptr<FlexibleModelFittingParameter> angle)
      : m_x(x),
        m_y(y),
        m_flux(flux),
        m_effective_radius(effective_radius),
        m_aspect_ratio(aspect_ratio),
        m_angle(angle) {}

  virtual ~FlexibleModelFittingDevaucouleursModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> reference_coordinates,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_x;
  std::shared_ptr<FlexibleModelFittingParameter> m_y;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
  std::shared_ptr<FlexibleModelFittingParameter> m_effective_radius;
  std::shared_ptr<FlexibleModelFittingParameter> m_aspect_ratio;
  std::shared_ptr<FlexibleModelFittingParameter> m_angle;
};

class FlexibleModelFittingSersicModel : public FlexibleModelFittingModel {
public:
  FlexibleModelFittingSersicModel(std::shared_ptr<FlexibleModelFittingParameter> x,
                                  std::shared_ptr<FlexibleModelFittingParameter> y,
                                  std::shared_ptr<FlexibleModelFittingParameter> flux,
                                  std::shared_ptr<FlexibleModelFittingParameter> sersic_index,
                                  std::shared_ptr<FlexibleModelFittingParameter> effective_radius,
                                  std::shared_ptr<FlexibleModelFittingParameter> aspect_ratio,
                                  std::shared_ptr<FlexibleModelFittingParameter> angle)
      : m_x(x),
        m_y(y),
        m_flux(flux),
        m_sersic_index(sersic_index),
        m_effective_radius(effective_radius),
        m_aspect_ratio(aspect_ratio),
        m_angle(angle) {}

  virtual ~FlexibleModelFittingSersicModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> reference_coordinates,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_x;
  std::shared_ptr<FlexibleModelFittingParameter> m_y;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
  std::shared_ptr<FlexibleModelFittingParameter> m_sersic_index;
  std::shared_ptr<FlexibleModelFittingParameter> m_effective_radius;
  std::shared_ptr<FlexibleModelFittingParameter> m_aspect_ratio;
  std::shared_ptr<FlexibleModelFittingParameter> m_angle;
};

class FlexibleModelFittingConstantModel : public FlexibleModelFittingModel {
public:
  explicit FlexibleModelFittingConstantModel(std::shared_ptr<FlexibleModelFittingParameter> value)
      : m_value(value) {}

  virtual ~FlexibleModelFittingConstantModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> reference_coordinates,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_value;
};

#ifdef WITH_ONNX_MODELS

class OnnxModel;

class FlexibleModelFittingOnnxModel : public FlexibleModelFittingModel {
public:
  FlexibleModelFittingOnnxModel(std::vector<std::shared_ptr<OnnxModel>> models,
                                std::shared_ptr<FlexibleModelFittingParameter> x,
                                std::shared_ptr<FlexibleModelFittingParameter> y,
                                std::shared_ptr<FlexibleModelFittingParameter> flux,
                                std::shared_ptr<FlexibleModelFittingParameter> aspect_ratio,
                                std::shared_ptr<FlexibleModelFittingParameter> angle,
                                std::shared_ptr<FlexibleModelFittingParameter> scale,
                                std::map<std::string, std::shared_ptr<FlexibleModelFittingParameter>> params);

  virtual ~FlexibleModelFittingOnnxModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> reference_coordinates,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_x;
  std::shared_ptr<FlexibleModelFittingParameter> m_y;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
  std::shared_ptr<FlexibleModelFittingParameter> m_aspect_ratio;
  std::shared_ptr<FlexibleModelFittingParameter> m_angle;
  std::shared_ptr<FlexibleModelFittingParameter> m_scale;

  std::map<std::string, std::shared_ptr<FlexibleModelFittingParameter>> m_params;

  std::vector<std::shared_ptr<OnnxModel>> m_models;
};

#endif

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_ */
