/*
 * FlexibleModelFittingModel.cpp
 *
 *  Created on: Oct 9, 2018
 *      Author: mschefer
 */

#include "ModelFitting/utils.h"

#include "ModelFitting/Models/TransformedModel.h"

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Engine/EngineParameterManager.h"

#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/OldSharp.h"
#include "ModelFitting/Models/AutoSharp.h"

#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingModel.h"

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

namespace SExtractor {

using namespace ModelFitting;

void FlexibleModelFittingPointModel::addForSource(const FlexibleModelFittingParameterManager& manager,
                                         const SourceInterface& source,
                                         std::vector<ModelFitting::PointModel>& point_models,
                                         std::vector<ModelFitting::TransformedModel>& extended_models,
                                         std::tuple<double, double, double, double> jacobian,
                                         std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const  {

  auto pixel_x = DependentParameter<BasicParameter, BasicParameter>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_x - offset.m_x;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));
  auto pixel_y = DependentParameter<BasicParameter, BasicParameter>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_y - offset.m_y;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));

  point_models.emplace_back(pixel_x, pixel_y, *manager.getParameter(source, m_flux));
}

void FlexibleModelFittingExponentialModel::addForSource(const FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::PointModel>& point_models,
                          std::vector<ModelFitting::TransformedModel>& extended_models,
                          std::tuple<double, double, double, double> jacobian,
                          std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const {

  auto pixel_x = DependentParameter<BasicParameter, BasicParameter>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_x - offset.m_x;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));
  auto pixel_y = DependentParameter<BasicParameter, BasicParameter>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_y - offset.m_y;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));


  ManualParameter n(1); // Sersic index for exponential
  ManualParameter x_scale(1); // we don't scale the x coordinate

  auto i0 = DependentParameter<BasicParameter, BasicParameter, BasicParameter>(
     [](double flux, double radius, double aspect) { return flux / (M_PI * 2.0 * 0.346 * radius * radius * aspect); },
     *manager.getParameter(source, m_flux), *manager.getParameter(source, m_effective_radius),
     *manager.getParameter(source, m_aspect_ratio));

  auto k = DependentParameter<BasicParameter>(
      [](double eff_radius) { return 1.7 / eff_radius; },
      *manager.getParameter(source, m_effective_radius));

  std::vector<std::unique_ptr<ModelComponent>> sersic_component;
  sersic_component.emplace_back(new SersicModelComponent(make_unique<OldSharp>(), i0, n, k));

  // FIXME this seems too arbitrary, what can we do that's better? use REff * constant?
  auto& boundaries = source.getProperty<PixelBoundaries>();
  int size = 2 * std::max(boundaries.getWidth(), boundaries.getHeight());

  extended_models.emplace_back(
      std::move(sersic_component), x_scale, *manager.getParameter(source, m_aspect_ratio), *manager.getParameter(source, m_angle),
      size, size, pixel_x, pixel_y, jacobian);
}

void FlexibleModelFittingDevaucouleursModel::addForSource(const FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::PointModel>& point_models,
                          std::vector<ModelFitting::TransformedModel>& extended_models,
                          std::tuple<double, double, double, double> jacobian,
                          std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const {

  auto pixel_x = DependentParameter<BasicParameter, BasicParameter>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_x - offset.m_x;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));
  auto pixel_y = DependentParameter<BasicParameter, BasicParameter>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_y - offset.m_y;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));


  ManualParameter n(4); // Sersic index for Devaucouleurs
  ManualParameter x_scale(1); // we don't scale the x coordinate

  auto i0 = DependentParameter<BasicParameter, BasicParameter, BasicParameter>(
     [](double flux, double radius, double aspect) { return flux * pow(10, 3.33) / (7.2 * M_PI * radius * radius *  aspect); },
     *manager.getParameter(source, m_flux), *manager.getParameter(source, m_effective_radius),
     *manager.getParameter(source, m_aspect_ratio));

  auto k = DependentParameter<BasicParameter>(
      [](double eff_radius) { return pow(3459.0 / eff_radius, .25); },
      *manager.getParameter(source, m_effective_radius));

  std::vector<std::unique_ptr<ModelComponent>> sersic_component;
  sersic_component.emplace_back(new SersicModelComponent(make_unique<OldSharp>(), i0, n, k));

  // FIXME this seems too arbitrary, what can we do that's better? use REff * constant?
  auto& boundaries = source.getProperty<PixelBoundaries>();
  int size = 2 * std::max(boundaries.getWidth(), boundaries.getHeight());

  extended_models.emplace_back(
      std::move(sersic_component), x_scale, *manager.getParameter(source, m_aspect_ratio), *manager.getParameter(source, m_angle),
      size, size, pixel_x, pixel_y, jacobian);
}


}


