/*
 * FlexibleModelFittingModel.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_

#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/TransformedModel.h"

#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

class FlexibleModelFittingParameterManager;

class FlexibleModelFittingModel {
public:
  virtual ~FlexibleModelFittingModel() {}

  virtual void addForSource(const FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<ModelFitting::TransformedModel>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const = 0;

private:
};

class FlexibleModelFittingPointModel : public FlexibleModelFittingModel {
public:
  FlexibleModelFittingPointModel(std::shared_ptr<FlexibleModelFittingParameter> alpha,
                                 std::shared_ptr<FlexibleModelFittingParameter> delta,
                                 std::shared_ptr<FlexibleModelFittingParameter> flux)
      : m_alpha_coord(alpha), m_delta_coord(delta), m_flux(flux) {}

  virtual ~FlexibleModelFittingPointModel() {}

  virtual void addForSource(const FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<ModelFitting::TransformedModel>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;
private:
  std::shared_ptr<FlexibleModelFittingParameter> m_alpha_coord;
  std::shared_ptr<FlexibleModelFittingParameter> m_delta_coord;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
};

class FlexibleModelFittingSersicModel : public FlexibleModelFittingModel {
public:
  FlexibleModelFittingSersicModel(std::shared_ptr<FlexibleModelFittingParameter> alpha,
                                  std::shared_ptr<FlexibleModelFittingParameter> delta,
                                  std::shared_ptr<FlexibleModelFittingParameter> flux,
                                  std::shared_ptr<FlexibleModelFittingParameter> effective_radius,
                                  std::shared_ptr<FlexibleModelFittingParameter> aspect_ratio,
                                  std::shared_ptr<FlexibleModelFittingParameter> angle,
                                  std::shared_ptr<FlexibleModelFittingParameter> n)
      : m_alpha_coord(alpha),
        m_delta_coord(delta),
        m_flux(flux),
        m_effective_radius(effective_radius),
        m_aspect_ratio(aspect_ratio),
        m_angle(angle),
        m_n(n) {}

  virtual ~FlexibleModelFittingSersicModel() {}

  virtual void addForSource(const FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<ModelFitting::TransformedModel>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_alpha_coord;
  std::shared_ptr<FlexibleModelFittingParameter> m_delta_coord;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
  std::shared_ptr<FlexibleModelFittingParameter> m_effective_radius;
  std::shared_ptr<FlexibleModelFittingParameter> m_aspect_ratio;
  std::shared_ptr<FlexibleModelFittingParameter> m_angle;
  std::shared_ptr<FlexibleModelFittingParameter> m_n;
};

class FlexibleModelFittingExponentialModel : public FlexibleModelFittingModel {
public:
  FlexibleModelFittingExponentialModel(std::shared_ptr<FlexibleModelFittingParameter> alpha,
                                  std::shared_ptr<FlexibleModelFittingParameter> delta,
                                  std::shared_ptr<FlexibleModelFittingParameter> flux,
                                  std::shared_ptr<FlexibleModelFittingParameter> effective_radius,
                                  std::shared_ptr<FlexibleModelFittingParameter> aspect_ratio,
                                  std::shared_ptr<FlexibleModelFittingParameter> angle)
      : m_alpha_coord(alpha),
        m_delta_coord(delta),
        m_flux(flux),
        m_effective_radius(effective_radius),
        m_aspect_ratio(aspect_ratio),
        m_angle(angle) {}

  virtual ~FlexibleModelFittingExponentialModel() {}

  virtual void addForSource(const FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<ModelFitting::TransformedModel>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_alpha_coord;
  std::shared_ptr<FlexibleModelFittingParameter> m_delta_coord;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
  std::shared_ptr<FlexibleModelFittingParameter> m_effective_radius;
  std::shared_ptr<FlexibleModelFittingParameter> m_aspect_ratio;
  std::shared_ptr<FlexibleModelFittingParameter> m_angle;
};

class FlexibleModelFittingDevaucouleursModel : public FlexibleModelFittingModel {
  public:
  FlexibleModelFittingDevaucouleursModel(std::shared_ptr<FlexibleModelFittingParameter> alpha,
                                  std::shared_ptr<FlexibleModelFittingParameter> delta,
                                  std::shared_ptr<FlexibleModelFittingParameter> flux,
                                  std::shared_ptr<FlexibleModelFittingParameter> effective_radius,
                                  std::shared_ptr<FlexibleModelFittingParameter> aspect_ratio,
                                  std::shared_ptr<FlexibleModelFittingParameter> angle)
      : m_alpha_coord(alpha),
        m_delta_coord(delta),
        m_flux(flux),
        m_effective_radius(effective_radius),
        m_aspect_ratio(aspect_ratio),
        m_angle(angle) {}

  virtual ~FlexibleModelFittingDevaucouleursModel() {}

  virtual void addForSource(const FlexibleModelFittingParameterManager& manager,
                            const SourceInterface& source,
                            std::vector<ModelFitting::PointModel>& point_models,
                            std::vector<ModelFitting::TransformedModel>& extended_models,
                            std::tuple<double, double, double, double> jacobian,
                            std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const;

private:
  std::shared_ptr<FlexibleModelFittingParameter> m_alpha_coord;
  std::shared_ptr<FlexibleModelFittingParameter> m_delta_coord;
  std::shared_ptr<FlexibleModelFittingParameter> m_flux;
  std::shared_ptr<FlexibleModelFittingParameter> m_effective_radius;
  std::shared_ptr<FlexibleModelFittingParameter> m_aspect_ratio;
  std::shared_ptr<FlexibleModelFittingParameter> m_angle;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGMODEL_H_ */
