/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics,
 * IAP-CNRS/Sorbonne Université
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
#include <algorithm>
#include <cmath>
#include <limits>

#include "AlexandriaKernel/memory_tools.h"

#include "ModelFitting/Engine/AsinhChiSquareComparator.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"
#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Engine/LeastSquareEngineManager.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"

#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinates.h"

#include "SEImplementation/Plugin/SpreadModel/SpreadModel.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModelTask.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModelUtils.h"
#include "SEUtils/IsNan.h"

#include "SEImplementation/Image/DownSampledImagePsf.h"

#include "SEImplementation/Plugin/SourcePsf/SourcePsfProperty.h"

namespace SourceXtractor {

using namespace ModelFitting;
using Euclid::make_unique;

namespace {

struct SourceModel {
  std::shared_ptr<EngineParameter> dx, dy;
  std::shared_ptr<DependentParameter<std::shared_ptr<EngineParameter>>> x, y;
  std::shared_ptr<EngineParameter> flux;

  SourceModel(double flux_guess, double x_guess, double y_guess,
              double pos_range)
      : dx(std::make_shared<EngineParameter>(
            0, make_unique<SigmoidConverter>(-pos_range, pos_range))),
        dy(std::make_shared<EngineParameter>(
            0, make_unique<SigmoidConverter>(-pos_range, pos_range))),

        x(createDependentParameter(
            [x_guess](double dx) { return dx + x_guess; }, dx)),
        y(createDependentParameter(
            [y_guess](double dy) { return dy + y_guess; }, dy)),
        flux(std::make_shared<EngineParameter>(
            flux_guess, make_unique<ExpSigmoidConverter>(flux_guess * .00001,
                                                         flux_guess * 1000))) {}

  void registerParameters(EngineParameterManager &manager) {
    manager.registerParameter(dx);
    manager.registerParameter(dy);
    manager.registerParameter(flux);
  }

  // Project the fitted reference position onto the measurement frame. WCS pixel
  // centres are integers; the renderer uses half-integers within the extracted stamp.
  PointModel createPointModelForFrame(
      const std::shared_ptr<CoordinateSystem> &reference_coordinates,
      const std::shared_ptr<CoordinateSystem> &frame_coordinates,
      PixelCoordinate offset) {

    auto frame_x = createDependentParameter(
        [reference_coordinates, frame_coordinates, offset](double x, double y) {
          return frame_coordinates
                     ->worldToImage(reference_coordinates->imageToWorld(
                         ImageCoordinate(x, y)))
                     .m_x -
                 offset.m_x + 0.5;
        },
        x, y);

    auto frame_y = createDependentParameter(
        [reference_coordinates, frame_coordinates, offset](double x, double y) {
          return frame_coordinates
                     ->worldToImage(reference_coordinates->imageToWorld(
                         ImageCoordinate(x, y)))
                     .m_y -
                 offset.m_y + 0.5;
        },
        x, y);

    return PointModel(frame_x, frame_y, flux);
  }


};

bool finite(double value) {
  return !fastmath_isnan(value) && !fastmath_isinf(value);
}

SpreadModel invalidResult() {
  const auto nan = std::numeric_limits<SeFloat>::quiet_NaN();
  return SpreadModel(nan, nan);
}

// Keep the mask and noise variances alongside the fit inputs for the final statistic.
struct FitStamp {
  std::shared_ptr<VectorImage<SeFloat>> image;
  std::shared_ptr<VectorImage<SeFloat>> weight;
  std::vector<double> variances;
  std::vector<bool> valid;
  std::size_t valid_pixels = 0;
};

double pixelVariance(SeFloat pixel_value, SeFloat background_variance, const MeasurementFrameInfo& info) {
  const SeFloat gain = info.getGain();
  const SeFloat saturation = info.getSaturation();
  if (!(finite(pixel_value) && finite(background_variance) && background_variance > 0 &&
        background_variance <= info.getVarianceThreshold() && !(saturation > 0 && pixel_value >= saturation))) {
    return 0;
  }
  // Negative background-subtracted values must not subtract shot-noise variance.
  const double variance = double(background_variance) + (gain > 0 ? std::max<double>(pixel_value, 0) / gain : 0);
  return finite(variance) && variance > 0 ? variance : 0;
}

FitStamp prepareFitStamp(const MeasurementFrameImages& images, const MeasurementFrameInfo& info,
                         PixelCoordinate offset, std::size_t width, std::size_t height) {
  FitStamp stamp;
  stamp.image = VectorImage<SeFloat>::create(images.getImageChunk(
      LayerSubtractedImage, offset.m_x, offset.m_y, width, height));
  stamp.weight = VectorImage<SeFloat>::create(width, height);
  stamp.variances.assign(width * height, 0);
  stamp.valid.assign(width * height, false);
  const auto frame_image = images.getLockedImage(LayerSubtractedImage);
  const auto variance_map = images.getLockedImage(LayerVarianceMap);
  // Share the pixel mask between fitting and spread calculation. The fitter takes
  // residual weights 1/sqrt(V); the statistic uses binary weights and keeps V separately.
  for (std::size_t y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < width; ++x) {
      const auto i = y * width + x;
      const double variance = pixelVariance(frame_image->getValue(offset.m_x + x, offset.m_y + y),
          variance_map->getValue(offset.m_x + x, offset.m_y + y), info);
      if (variance > 0) {
        stamp.variances[i] = variance;
        stamp.valid[i] = true;
        ++stamp.valid_pixels;
        stamp.weight->at(x, y) = std::sqrt(1.0 / variance);
      }
      else {
        stamp.weight->at(x, y) = 0;
        // A zero weight alone does not prevent NaN residuals (NaN * 0).
        stamp.image->at(x, y) = 0;
      }
    }
  }
  return stamp;
}

bool centreInFrame(const PointModel& point, PixelCoordinate offset, const MeasurementFrameInfo& info) {
  // Model coordinates have pixel centres at half integers. Restore the
  // full-frame coordinates and test against pixel edges, not stamp edges.
  const double x = point.getX() + offset.m_x;
  const double y = point.getY() + offset.m_y;
  return finite(x) && finite(y) && x >= 0 && y >= 0 && x < info.getWidth() && y < info.getHeight();
}

bool fitPointSource(SourceModel& source_model, PointModel point, FitStamp& stamp,
                    const DownSampledImagePsf& psf, const std::string& engine_name, unsigned int max_iterations) {
  using VectorImageType = std::shared_ptr<VectorImage<SeFloat>>;
  std::vector<PointModel> point_models;
  point_models.emplace_back(std::move(point));
  FrameModel<DownSampledImagePsf, VectorImageType> frame_model{
      1.0, std::size_t(stamp.image->getWidth()), std::size_t(stamp.image->getHeight()), {}, std::move(point_models), {}, psf};
  ResidualEstimator res_estimator;
  res_estimator.registerBlockProvider(createDataVsModelResiduals(
      stamp.image, std::move(frame_model), stamp.weight, AsinhChiSquareComparator{}));
  EngineParameterManager manager;
  source_model.registerParameters(manager);
  auto engine = LeastSquareEngineManager::create(engine_name, max_iterations);
  const auto solution = engine->solveProblem(manager, res_estimator);
  // Finite parameters can survive a failed solve. Explicit failures invalidate
  // this frame; reaching the iteration limit alone remains eligible.
  return solution.status_flag != LeastSquareSummary::ERROR && solution.status_flag != LeastSquareSummary::MEMORY;
}

} // namespace

double SpreadModelTask::computeDownScaling(SourceInterface& source) const {
  // Match iterative fitting: estimate the largest stamp area in PSF pixels
  // and use one rendering scale for all measurement frames.
  double fit_size = 0;
  for (auto frame_index : m_measurement_frames) {
    const auto &frame_rect =
        source.getProperty<MeasurementFrameRectangle>(frame_index);
    if (!frame_rect.isValid() || frame_rect.isEmpty()) {
      continue;
    }
    const auto &psf_property =
        source.getProperty<SourcePsfProperty>(frame_index);
    if (!psf_property.getPsf()) {
      continue;
    }
    const auto sampling = psf_property.getPixelSampling();
    fit_size =
        std::max(fit_size, frame_rect.getWidth() * frame_rect.getHeight() /
                               (sampling * sampling));
  }

  double down_scaling = m_scale_factor;
  if (fit_size > 2.0 * m_max_fit_area) {
    down_scaling *= std::sqrt(m_max_fit_area / fit_size);
  }

  return down_scaling;
}

SpreadModel SpreadModelTask::computeFrameSpread(SourceInterface& source, double down_scaling) const {
  const auto& frame_rect = source.getProperty<MeasurementFrameRectangle>(m_instance);
  if (!frame_rect.isValid() || frame_rect.isEmpty()) {
    return invalidResult();
  }
  // Point-source rendering requires a PSF.
  const auto& psf_property = source.getProperty<SourcePsfProperty>(m_instance);
  if (!psf_property.getPsf()) {
    return invalidResult();
  }
  // Measure the original local PSF before any rendering downsampling changes its profile.
  const double fwhm = SpreadModelUtils::computePsfFwhm(*psf_property.getPsf(), psf_property.getPixelSampling());
  if (!finite(fwhm) || fwhm <= 0) {
    return invalidResult();
  }
  const auto frame_coordinates = source.getProperty<MeasurementFrameCoordinates>(m_instance).getCoordinateSystem();
  const auto& frame_images = source.getProperty<MeasurementFrameImages>(m_instance);
  const auto& frame_info = source.getProperty<MeasurementFrameInfo>(m_instance);
  const PixelCoordinate offset(frame_rect.getTopLeft().m_x, frame_rect.getTopLeft().m_y);
  const auto width = std::size_t(frame_rect.getWidth());
  const auto height = std::size_t(frame_rect.getHeight());
  if (width == 0 || height == 0) {
    return invalidResult();
  }
  auto stamp = prepareFitStamp(frame_images, frame_info, offset, width, height);
  const auto& centroid = source.getProperty<PixelCentroid>();
  const auto iso_flux = source.getProperty<IsophotalFlux>().getFlux();
  const double guess_x = centroid.getCentroidX();
  const double guess_y = centroid.getCentroidY();
  // The point fit has three free parameters (x, y, flux); require at least
  // one residual degree of freedom after masking.
  if (stamp.valid_pixels < 4 || !finite(iso_flux) || !finite(guess_x) || !finite(guess_y)) {
    return invalidResult();
  }
  // FIXME: temporary hardcoded floor, matching PythonConfig/ObjectInfo.cpp.
  // Keep the initial flux positive for ExpSigmoidConverter; this should be configurable.
  const auto flux_guess = std::max<double>(iso_flux, 0.0001);
  SourceModel source_model(flux_guess, guess_x, guess_y, 5.0 /* FIXME radius_guess * 2 */);
  const auto reference_coordinates = source.getProperty<ReferenceCoordinates>().getCoordinateSystem();
  auto point = source_model.createPointModelForFrame(reference_coordinates, frame_coordinates, offset);
  if (!centreInFrame(point, offset, frame_info)) {
    return invalidResult();
  }
  DownSampledImagePsf psf(psf_property.getPixelSampling(), psf_property.getPsf(), down_scaling,
                          m_should_renormalize.at(m_instance));
  if (!fitPointSource(source_model, std::move(point), stamp, psf, m_least_squares_engine, m_max_iterations)) {
    return invalidResult();
  }
  // Snapshot the fitted position in exactly the same pixel convention as the point fit.
  const auto fitted_point = source_model.createPointModelForFrame(reference_coordinates, frame_coordinates, offset);
  const auto flux = source_model.flux->getValue();
  if (!finite(flux) || flux <= 0 || !centreInFrame(fitted_point, offset, frame_info)) {
    return invalidResult();
  }
  const auto stamps = SpreadModelUtils::renderModels(fitted_point.getX(), fitted_point.getY(), flux, fwhm,
                                                    width, height, psf);
  return SpreadModelUtils::computeSpread(*stamps.first, *stamps.second, *stamp.image, stamp.variances, stamp.valid);
}

void SpreadModelTask::computeProperties(SourceInterface& source) const {
  const auto result = computeFrameSpread(source, computeDownScaling(source));
  source.setIndexedProperty<SpreadModel>(m_instance, result.getSpreadModel(), result.getSpreadModelError());
}

} // namespace SourceXtractor
