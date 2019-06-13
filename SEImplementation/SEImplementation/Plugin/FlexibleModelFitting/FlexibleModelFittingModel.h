/*
 * FlexibleModelFittingModel.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_

#include "ModelFitting/Models/ConstantModel.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/TransformedModel.h"

#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

class FlexibleModelFittingParameterManager;

class FlexibleModelFittingModel {
public:
  virtual ~FlexibleModelFittingModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<ModelFitting::TransformedModel>& extended_models,
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
                            std::vector<ModelFitting::TransformedModel>& extended_models,
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
                            std::vector<ModelFitting::TransformedModel>& extended_models,
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
                            std::vector<ModelFitting::TransformedModel>& extended_models,
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
                            std::vector<ModelFitting::TransformedModel>& extended_models,
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
  FlexibleModelFittingConstantModel(std::shared_ptr<FlexibleModelFittingParameter> value)
      : m_value(value) {}

  virtual ~FlexibleModelFittingConstantModel() {}

  virtual void addForSource(FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::ConstantModel>& constant_models,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<ModelFitting::TransformedModel>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> reference_coordinates,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_value;
};


}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_ */
