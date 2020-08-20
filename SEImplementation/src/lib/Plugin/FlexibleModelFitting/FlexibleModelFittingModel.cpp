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
 * FlexibleModelFittingModel.cpp
 *
 *  Created on: Oct 9, 2018
 *      Author: mschefer
 */

#include "AlexandriaKernel/memory_tools.h"

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

#include "ModelFitting/Models/CompactExponentialModel.h"
#include "ModelFitting/Models/CompactSersicModel.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"

#include "Configuration/ConfigManager.h"
#include "SEImplementation/Configuration/SamplingConfig.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingModel.h"

namespace SourceXtractor {

using namespace ModelFitting;
using Euclid::make_unique;
using namespace Euclid::Configuration;

static const double MODEL_MIN_SIZE = 4.0;
static const double MODEL_SIZE_FACTOR = 1.2;

// Reference for Sersic related quantities:
// See https://ned.ipac.caltech.edu/level5/March05/Graham/Graham2.html


// Note about pixel coordinates:

// The model fitting is made in pixel coordinates of the detection image

// Internally we use a coordinate system with (0,0) at the center of the first pixel. But for compatibility with
// SExtractor 2, all pixel coordinates visible to the end user need to follow the FITS convention of (1,1) being the
// center of the coordinate system.

// The ModelFitting module uses the more common standard of (0, 0) being the corner of the first pixel.

// So we first convert the Python parameter to our internal coordinates, then do the transformation of coordinate,
// subtract the offset to the image cut-out and shift the result by 0.5 pixels


void FlexibleModelFittingPointModel::addForSource(FlexibleModelFittingParameterManager& manager,
                                         const SourceInterface& source,
                                         std::vector<ModelFitting::ConstantModel>& /* constant_models */,
                                         std::vector<ModelFitting::PointModel>& point_models,
                                         std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>&,
                                         std::tuple<double, double, double, double> /*jacobian*/,
                                         std::shared_ptr<CoordinateSystem> reference_coordinates,
                                         std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const  {

  //auto pixel_x = std::make_shared<DependentParameter<std::shared_ptr<BasicParameter>, std::shared_ptr<BasicParameter>>>(

  auto pixel_x = createDependentParameter(
      [reference_coordinates, coordinates, offset](double x, double y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld(ImageCoordinate(x-1, y-1))).m_x - offset.m_x + 0.5;
      }, manager.getParameter(source, m_x), manager.getParameter(source, m_y));


  auto pixel_y = createDependentParameter(
      [reference_coordinates, coordinates, offset](double x, double y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld(ImageCoordinate(x-1, y-1))).m_y - offset.m_y + 0.5;
      }, manager.getParameter(source, m_x), manager.getParameter(source, m_y));

  point_models.emplace_back(pixel_x, pixel_y, manager.getParameter(source, m_flux));
}

void FlexibleModelFittingExponentialModel::addForSource(FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::ConstantModel>& /* constant_models */,
                          std::vector<ModelFitting::PointModel>& /*point_models*/,
                          std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                          std::tuple<double, double, double, double> jacobian,
                          std::shared_ptr<CoordinateSystem> reference_coordinates,
                          std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const {

  auto pixel_x = createDependentParameter(
      [reference_coordinates, coordinates, offset](double x, double y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld(ImageCoordinate(x-1, y-1))).m_x - offset.m_x + 0.5;
      }, manager.getParameter(source, m_x), manager.getParameter(source, m_y));


  auto pixel_y = createDependentParameter(
      [reference_coordinates, coordinates, offset](double x, double y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld(ImageCoordinate(x-1, y-1))).m_y - offset.m_y + 0.5;
      }, manager.getParameter(source, m_x), manager.getParameter(source, m_y));

  //auto n = std::make_shared<ManualParameter>(1); // Sersic index for exponential
  auto x_scale = std::make_shared<ManualParameter>(1); // we don't scale the x coordinate

//  ManualParameter x_scale(1); // we don't scale the x coordinate
  auto i0 = createDependentParameter(
     [](double flux, double radius, double aspect) { return flux / (2 * M_PI * 0.35513 * radius * radius * aspect); },
     manager.getParameter(source, m_flux), manager.getParameter(source, m_effective_radius),
     manager.getParameter(source, m_aspect_ratio));

  auto k = createDependentParameter(
      [](double eff_radius) { return 1.678 / eff_radius; },
      manager.getParameter(source, m_effective_radius));

  auto& boundaries = source.getProperty<PixelBoundaries>();
  int size = std::max(MODEL_MIN_SIZE, MODEL_SIZE_FACTOR * std::max(boundaries.getWidth(), boundaries.getHeight()));

  extended_models.emplace_back(std::make_shared<CompactExponentialModel<ImageInterfaceTypePtr>>(
      2.0, i0, k, x_scale, manager.getParameter(source, m_aspect_ratio), manager.getParameter(source, m_angle),
      size, size, pixel_x, pixel_y, jacobian));
}

void FlexibleModelFittingDevaucouleursModel::addForSource(FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::ConstantModel>& /* constant_models */,
                          std::vector<ModelFitting::PointModel>& /*point_models*/,
                          std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                          std::tuple<double, double, double, double> jacobian,
                          std::shared_ptr<CoordinateSystem> reference_coordinates,
                          std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const {

  auto pixel_x = createDependentParameter(
      [reference_coordinates, coordinates, offset](double x, double y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld(ImageCoordinate(x-1, y-1))).m_x - offset.m_x + 0.5;
      }, manager.getParameter(source, m_x), manager.getParameter(source, m_y));


  auto pixel_y = createDependentParameter(
      [reference_coordinates, coordinates, offset](double x, double y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld(ImageCoordinate(x-1, y-1))).m_y - offset.m_y + 0.5;
      }, manager.getParameter(source, m_x), manager.getParameter(source, m_y));

  auto n = std::make_shared<ManualParameter>(4); // Sersic index for Devaucouleurs
  auto x_scale = std::make_shared<ManualParameter>(1); // we don't scale the x coordinate

  auto i0 = createDependentParameter(
     [](double flux, double radius, double aspect) { return flux / (2 * M_PI * 0.001684925 * radius * radius * aspect); },
     manager.getParameter(source, m_flux), manager.getParameter(source, m_effective_radius),
     manager.getParameter(source, m_aspect_ratio));

  auto k = createDependentParameter(
      [](double eff_radius) { return 7.669 / pow(eff_radius, .25); },
      manager.getParameter(source, m_effective_radius));

  auto& boundaries = source.getProperty<PixelBoundaries>();
  int size = std::max(MODEL_MIN_SIZE, MODEL_SIZE_FACTOR * std::max(boundaries.getWidth(), boundaries.getHeight()));

  auto& config_manager = ConfigManager::getInstance(0);
  auto& sampling_config = config_manager.getConfiguration<SamplingConfig>();
  auto sampling_method = sampling_config.getSamplingMethod();
  auto sample_nb = sampling_config.getSampleNb();
  auto adaptive_target = sampling_config.getAdaptiveTargetPrecision();

  if (sampling_config.getSamplingMethod() == SamplingConfig::SamplingMethod::LEGACY) {
    std::vector<std::unique_ptr<ModelComponent>> sersic_component;
    sersic_component.emplace_back(new SersicModelComponent(make_unique<OldSharp>(), i0, n, k));

    extended_models.emplace_back(std::make_shared<TransformedModel<ImageInterfaceTypePtr>>(
      std::move(sersic_component), x_scale, manager.getParameter(source, m_aspect_ratio),
      manager.getParameter(source, m_angle), size, size, pixel_x, pixel_y, jacobian));
  } else {
    extended_models.emplace_back(std::make_shared<CompactSersicModel<ImageInterfaceTypePtr>>(
        3.0, i0, k, n, x_scale, manager.getParameter(source, m_aspect_ratio), manager.getParameter(source, m_angle),
        size, size, pixel_x, pixel_y, jacobian, CompactSersicModel<ImageInterfaceTypePtr>::SamplingMethod(sampling_method), sample_nb, adaptive_target));
  }
}

static double computeBn(double n) {
  // Using approximation from MacArthur, L.A., Courteau, S., & Holtzman, J.A. 2003, ApJ, 582, 689
  return 2 * n - 1.0 / 3.0 + 4 / (405 * n)
      + 46 / (25515 * n * n) + 131 / (1148175 * n * n * n) - 2194697 / (30690717750 * n * n * n * n);
}

void FlexibleModelFittingSersicModel::addForSource(FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::ConstantModel>& /* constant_models */,
                          std::vector<ModelFitting::PointModel>& /*point_models*/,
                          std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>& extended_models,
                          std::tuple<double, double, double, double> jacobian,
                          std::shared_ptr<CoordinateSystem> reference_coordinates,
                          std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset) const {
  auto pixel_x = createDependentParameter(
      [reference_coordinates, coordinates, offset](double x, double y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld(ImageCoordinate(x-1, y-1))).m_x - offset.m_x + 0.5;
      }, manager.getParameter(source, m_x), manager.getParameter(source, m_y));


  auto pixel_y = createDependentParameter(
      [reference_coordinates, coordinates, offset](double x, double y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld(ImageCoordinate(x-1, y-1))).m_y - offset.m_y + 0.5;
      }, manager.getParameter(source, m_x), manager.getParameter(source, m_y));

  auto x_scale = std::make_shared<ManualParameter>(1); // we don't scale the x coordinate

  auto i0 = createDependentParameter(
      [](double flux, double radius, double aspect, double n) { return flux / (2 * M_PI * pow(computeBn(n), -2*n) * n * std::tgamma(2*n) * radius * radius *  aspect); },
     manager.getParameter(source, m_flux), manager.getParameter(source, m_effective_radius),
     manager.getParameter(source, m_aspect_ratio), manager.getParameter(source, m_sersic_index));

  auto k = createDependentParameter(
      [](double eff_radius, double n) { return computeBn(n) / pow(eff_radius, 1.0 / n); },
      manager.getParameter(source, m_effective_radius), manager.getParameter(source, m_sersic_index));

  auto& boundaries = source.getProperty<PixelBoundaries>();
  int size = std::max(MODEL_MIN_SIZE, MODEL_SIZE_FACTOR * std::max(boundaries.getWidth(), boundaries.getHeight()));

  auto& config_manager = ConfigManager::getInstance(0);
  auto& sampling_config = config_manager.getConfiguration<SamplingConfig>();
  auto sampling_method = sampling_config.getSamplingMethod();
  auto sample_nb = sampling_config.getSampleNb();
  auto adaptive_target = sampling_config.getAdaptiveTargetPrecision();

  if (sampling_config.getSamplingMethod() == SamplingConfig::SamplingMethod::LEGACY) {
    std::vector<std::unique_ptr<ModelComponent>> sersic_component;
    sersic_component.emplace_back(new SersicModelComponent(make_unique<OldSharp>(), i0,
                                                           manager.getParameter(source, m_sersic_index), k));

    extended_models.emplace_back(std::make_shared<TransformedModel<ImageInterfaceTypePtr>>(
      std::move(sersic_component), x_scale, manager.getParameter(source, m_aspect_ratio),
      manager.getParameter(source, m_angle), size, size, pixel_x, pixel_y, jacobian));
  } else {
    extended_models.emplace_back(std::make_shared<CompactSersicModel<ImageInterfaceTypePtr>>(
        3.0, i0, k, manager.getParameter(source, m_sersic_index), x_scale, manager.getParameter(source, m_aspect_ratio),
        manager.getParameter(source, m_angle), size, size, pixel_x, pixel_y, jacobian,
        CompactSersicModel<ImageInterfaceTypePtr>::SamplingMethod(sampling_method), sample_nb, adaptive_target));
  }
}

void FlexibleModelFittingConstantModel::addForSource(FlexibleModelFittingParameterManager& manager,
                          const SourceInterface& source,
                          std::vector<ModelFitting::ConstantModel>& constant_models,
                          std::vector<ModelFitting::PointModel>& /* point_models */,
                          std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>&,
                          std::tuple<double, double, double, double> /* jacobian */,
                          std::shared_ptr<CoordinateSystem> /* reference_coordinates */,
                          std::shared_ptr<CoordinateSystem> /* coordinates */, PixelCoordinate /* offset */) const {
  constant_models.emplace_back(manager.getParameter(source, m_value));
}

}

