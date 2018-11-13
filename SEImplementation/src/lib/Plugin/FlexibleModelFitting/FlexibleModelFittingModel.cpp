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

// Reference for Sersic related quantities:
// See https://ned.ipac.caltech.edu/level5/March05/Graham/Graham2.html

void FlexibleModelFittingPointModel::addForSource(FlexibleModelFittingParameterManager& manager,
                                         const SourceInterface& source,
                                         std::vector<ModelFitting::PointModel>& point_models,
                                         std::vector<ModelFitting::TransformedModel>& /*extended_models*/,
                                         std::tuple<double, double, double, double> /*jacobian*/,
                                         std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const  {

  auto pixel_x = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_x - offset.m_x;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));
  auto pixel_y = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_y - offset.m_y;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));

  manager.storeParameter(pixel_x);
  manager.storeParameter(pixel_y);
  point_models.emplace_back(*pixel_x, *pixel_y, *manager.getParameter(source, m_flux));
}


void FlexibleModelFittingExponentialModel::addForSource(FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::PointModel>& /*point_models*/,
                          std::vector<ModelFitting::TransformedModel>& extended_models,
                          std::tuple<double, double, double, double> jacobian,
                          std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const {

  auto pixel_x = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_x - offset.m_x;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));
  auto pixel_y = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_y - offset.m_y;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));


  ManualParameter n(1); // Sersic index for exponential
  ManualParameter x_scale(1); // we don't scale the x coordinate

  auto i0 = std::make_shared<DependentParameter<BasicParameter, BasicParameter, BasicParameter>>(
     [](double flux, double radius, double aspect) { return flux / (2 * M_PI * 0.35513 * radius * radius * aspect); },
     *manager.getParameter(source, m_flux), *manager.getParameter(source, m_effective_radius),
     *manager.getParameter(source, m_aspect_ratio));

  auto k = std::make_shared<DependentParameter<BasicParameter>>(
      [](double eff_radius) { return 1.678 / eff_radius; },
      *manager.getParameter(source, m_effective_radius));

  manager.storeParameter(pixel_x);
  manager.storeParameter(pixel_y);
  manager.storeParameter(i0);
  manager.storeParameter(k);

  std::vector<std::unique_ptr<ModelComponent>> sersic_component;
  sersic_component.emplace_back(new SersicModelComponent(make_unique<OldSharp>(), *i0, n, *k));

  // FIXME this seems too arbitrary, what can we do that's better? use REff * constant?
  auto& boundaries = source.getProperty<PixelBoundaries>();
  int size = 2 * std::max(boundaries.getWidth(), boundaries.getHeight());

  extended_models.emplace_back(
      std::move(sersic_component), x_scale, *manager.getParameter(source, m_aspect_ratio), *manager.getParameter(source, m_angle),
      size, size, *pixel_x, *pixel_y, jacobian);
}

void FlexibleModelFittingDevaucouleursModel::addForSource(FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::PointModel>& /*point_models*/,
                          std::vector<ModelFitting::TransformedModel>& extended_models,
                          std::tuple<double, double, double, double> jacobian,
                          std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const {

  auto pixel_x = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_x - offset.m_x;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));
  auto pixel_y = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_y - offset.m_y;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));


  ManualParameter n(4); // Sersic index for Devaucouleurs
  ManualParameter x_scale(1); // we don't scale the x coordinate

  auto i0 = std::make_shared<DependentParameter<BasicParameter, BasicParameter, BasicParameter>>(
     [](double flux, double radius, double aspect) { return flux / (2 * M_PI * 1.684925 * radius * radius *  aspect); },
     *manager.getParameter(source, m_flux), *manager.getParameter(source, m_effective_radius),
     *manager.getParameter(source, m_aspect_ratio));

  auto k = std::make_shared<DependentParameter<BasicParameter>>(
      [](double eff_radius) { return 7.669 / pow(eff_radius, .25); },
      *manager.getParameter(source, m_effective_radius));

  manager.storeParameter(pixel_x);
  manager.storeParameter(pixel_y);
  manager.storeParameter(i0);
  manager.storeParameter(k);

  std::vector<std::unique_ptr<ModelComponent>> sersic_component;
  sersic_component.emplace_back(new SersicModelComponent(make_unique<OldSharp>(), *i0, n, *k));

  // FIXME this seems too arbitrary, what can we do that's better? use REff * constant?
  auto& boundaries = source.getProperty<PixelBoundaries>();
  int size = 2 * std::max(boundaries.getWidth(), boundaries.getHeight());

  extended_models.emplace_back(
      std::move(sersic_component), x_scale, *manager.getParameter(source, m_aspect_ratio), *manager.getParameter(source, m_angle),
      size, size, *pixel_x, *pixel_y, jacobian);
}

static double computeBn(double n) {
  // Using approximation from MacArthur, L.A., Courteau, S., & Holtzman, J.A. 2003, ApJ, 582, 689
  // FIXME solve for increased precision? or is it too slow?
  if (n <= 0.36) {
    return 0.01945 - 0.8902 * n + 10.95 * n * n - 19.67 * n * n * n + 13.43 * n * n * n * n;
  } else {
    return 2 * n - 1.0 / 3.0 + 4 / (405 * n)
        + 46 / (25515 * n * n) + 131 / (1148175 * n * n * n) - 2194697 / (30690717750 * n * n * n * n);
  }
}

void FlexibleModelFittingSersicModel::addForSource(FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::PointModel>& /*point_models*/,
                          std::vector<ModelFitting::TransformedModel>& extended_models,
                          std::tuple<double, double, double, double> jacobian,
                          std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const {

  auto pixel_x = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_x - offset.m_x;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));
  auto pixel_y = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [coordinates, offset](double alpha, double delta) {
        return coordinates->worldToImage(WorldCoordinate(alpha, delta)).m_y - offset.m_y;
      }, *manager.getParameter(source, m_alpha_coord), *manager.getParameter(source, m_delta_coord));

  ManualParameter x_scale(1); // we don't scale the x coordinate

  auto i0 = std::make_shared<DependentParameter<BasicParameter, BasicParameter, BasicParameter, BasicParameter>>(
      [](double flux, double radius, double aspect, double n) { return flux / (2 * M_PI * pow(computeBn(n), -2*n) * n * std::tgamma(2*n) * radius * radius *  aspect); },
     *manager.getParameter(source, m_flux), *manager.getParameter(source, m_effective_radius),
     *manager.getParameter(source, m_aspect_ratio), *manager.getParameter(source, m_sersic_index));

  auto k = std::make_shared<DependentParameter<BasicParameter, BasicParameter>>(
      [](double eff_radius, double n) { return computeBn(n) * pow(eff_radius, 1.0 / n); },
      *manager.getParameter(source, m_effective_radius), *manager.getParameter(source, m_sersic_index));

  std::vector<std::unique_ptr<ModelComponent>> sersic_component;
  sersic_component.emplace_back(new SersicModelComponent(make_unique<OldSharp>(), *i0, *manager.getParameter(source, m_sersic_index), *k));

  manager.storeParameter(pixel_x);
  manager.storeParameter(pixel_y);
  manager.storeParameter(i0);
  manager.storeParameter(k);

  // FIXME this seems too arbitrary, what can we do that's better? use REff * constant?
  auto& boundaries = source.getProperty<PixelBoundaries>();
  int size = 2 * std::max(boundaries.getWidth(), boundaries.getHeight());

  extended_models.emplace_back(
      std::move(sersic_component), x_scale, *manager.getParameter(source, m_aspect_ratio), *manager.getParameter(source, m_angle),
      size, size, *pixel_x, *pixel_y, jacobian);
}



}


