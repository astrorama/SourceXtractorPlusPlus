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

} // namespace

void SpreadModelTask::computeProperties(SourceInterface &source) const {
  typedef std::shared_ptr<VectorImage<SourceXtractor::SeFloat>> VectorImageType;

  auto &pixel_centroid = source.getProperty<PixelCentroid>();
  auto iso_flux = source.getProperty<IsophotalFlux>().getFlux();

  double pixel_scale = 1;

  // Position is fit in the reference frame's absolute pixel coordinates, then
  // projected onto each measurement frame
  double guess_x = pixel_centroid.getCentroidX();
  double guess_y = pixel_centroid.getCentroidY();

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

  {
    const auto frame_index = m_instance;
    ResidualEstimator res_estimator{};
    EngineParameterManager manager{};

    auto &frame_rect =
        source.getProperty<MeasurementFrameRectangle>(frame_index);
    if (!frame_rect.isValid() || frame_rect.isEmpty()) {
      source.setIndexedProperty<SpreadModel>(frame_index,
          std::numeric_limits<SeFloat>::quiet_NaN(), std::numeric_limits<SeFloat>::quiet_NaN());
      return;
    }

    // Point-source rendering requires a PSF
    const auto &psf_property =
        source.getProperty<SourcePsfProperty>(frame_index);
    if (!psf_property.getPsf()) {
      source.setIndexedProperty<SpreadModel>(frame_index,
          std::numeric_limits<SeFloat>::quiet_NaN(), std::numeric_limits<SeFloat>::quiet_NaN());
      return;
    }

    // Measure the original local PSF before any rendering downsampling changes its profile.
    const double fwhm = SpreadModelUtils::computePsfFwhm(*psf_property.getPsf(), psf_property.getPixelSampling());
    if (fastmath_isnan(fwhm) || fastmath_isinf(fwhm) || fwhm <= 0) {
      source.setIndexedProperty<SpreadModel>(frame_index,
          std::numeric_limits<SeFloat>::quiet_NaN(), std::numeric_limits<SeFloat>::quiet_NaN());
      return;
    }

    auto frame_coordinates =
        source.getProperty<MeasurementFrameCoordinates>(frame_index)
            .getCoordinateSystem();
    auto &frame_images =
        source.getProperty<MeasurementFrameImages>(frame_index);
    auto &frame_info = source.getProperty<MeasurementFrameInfo>(frame_index);

    PixelCoordinate offset(frame_rect.getTopLeft().m_x,
                           frame_rect.getTopLeft().m_y);
    size_t width = (size_t)frame_rect.getWidth();
    size_t height = (size_t)frame_rect.getHeight();

    if (width == 0 || height == 0) {
      source.setIndexedProperty<SpreadModel>(frame_index,
          std::numeric_limits<SeFloat>::quiet_NaN(), std::numeric_limits<SeFloat>::quiet_NaN());
      return;
    }
    auto image = VectorImage<SeFloat>::create(frame_images.getImageChunk(
        LayerSubtractedImage, offset.m_x, offset.m_y, width, height));

    auto frame_image = frame_images.getLockedImage(LayerSubtractedImage);
    auto variance_map = frame_images.getLockedImage(LayerVarianceMap);

    SeFloat gain = frame_info.getGain();
    SeFloat saturation = frame_info.getSaturation();

    // Share the pixel mask between fitting and spread calculation. The fitter takes
    // residual weights 1/sqrt(V); the statistic uses binary weights and keeps V separately.
    auto weight = VectorImage<SeFloat>::create(width, height);
    std::vector<double> variances(width * height, 0);
    std::vector<bool> valid(width * height, false);
    std::size_t valid_pixels = 0;
    auto finite = [](double value) { return !fastmath_isnan(value) && !fastmath_isinf(value); };
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        auto back_var = variance_map->getValue(offset.m_x + x, offset.m_y + y);
        auto pixel_val = frame_image->getValue(offset.m_x + x, offset.m_y + y);
        const auto i = y * width + x;
        if (finite(pixel_val) && finite(back_var) && back_var > 0 &&
            back_var <= frame_info.getVarianceThreshold() && !(saturation > 0 && pixel_val >= saturation)) {
          // Negative background-subtracted values must not subtract shot-noise variance.
          const double variance = double(back_var) + (gain > 0 ? std::max<double>(pixel_val, 0) / gain : 0);
          if (finite(variance) && variance > 0) {
            variances[i] = variance;
            valid[i] = true;
            ++valid_pixels;
            weight->at(x, y) = std::sqrt(1.0 / variance);
          }
        }
        if (!valid[i]) {
          weight->at(x, y) = 0;
          // A zero weight alone does not prevent NaN residuals (NaN * 0).
          image->at(x, y) = 0;
        }
      }
    }

    // The point fit has three free parameters (x, y, flux); require at least
    // one residual degree of freedom after masking.
    if (valid_pixels < 4 || !finite(iso_flux) || !finite(guess_x) || !finite(guess_y)) {
      source.setIndexedProperty<SpreadModel>(frame_index,
          std::numeric_limits<SeFloat>::quiet_NaN(), std::numeric_limits<SeFloat>::quiet_NaN());
      return;
    }

    // FIXME: temporary hardcoded floor, matching PythonConfig/ObjectInfo.cpp.
    // Keep the initial flux positive for ExpSigmoidConverter; this should be
    // configurable.
    auto flux_guess = std::max<double>(iso_flux, 0.0001);

    auto source_model = make_unique<SourceModel>(
        flux_guess, guess_x, guess_y, 5.0 /* FIXME radius_guess * 2*/);

    std::vector<PointModel> point_models;
    auto reference_coordinates =
        source.getProperty<ReferenceCoordinates>().getCoordinateSystem();
    point_models.emplace_back(source_model->createPointModelForFrame(
        reference_coordinates, frame_coordinates, offset));

    auto centre_in_frame = [&](const PointModel& point) {
      // Model coordinates have pixel centres at half integers. Restore the
      // full-frame coordinates and test against pixel edges, not stamp edges.
      const double x = point.getX() + offset.m_x;
      const double y = point.getY() + offset.m_y;
      return finite(x) && finite(y) && x >= 0 && y >= 0 &&
             x < frame_info.getWidth() && y < frame_info.getHeight();
    };
    if (!centre_in_frame(point_models.front())) {
      source.setIndexedProperty<SpreadModel>(frame_index,
          std::numeric_limits<SeFloat>::quiet_NaN(), std::numeric_limits<SeFloat>::quiet_NaN());
      return;
    }

    DownSampledImagePsf psf(psf_property.getPixelSampling(),
                            psf_property.getPsf(), down_scaling,
                            m_should_renormalize.at(frame_index));

    FrameModel<DownSampledImagePsf, VectorImageType> frame_model{
        pixel_scale,
        width,
        height,
        std::vector<ConstantModel>{},
        std::move(point_models),
        std::vector<std::shared_ptr<
            ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>>{},
        psf};

    auto data_vs_model = createDataVsModelResiduals(
        image, std::move(frame_model), weight, AsinhChiSquareComparator{});
    res_estimator.registerBlockProvider(std::move(data_vs_model));

    source_model->registerParameters(manager);

    // Perform the minimization
    auto engine = LeastSquareEngineManager::create(m_least_squares_engine,
                                                   m_max_iterations);
    const auto solution = engine->solveProblem(manager, res_estimator);
    // Finite parameters can survive a failed solve. Explicit failures invalidate
    // this frame; reaching the iteration limit alone remains eligible.
    if (solution.status_flag == LeastSquareSummary::ERROR || solution.status_flag == LeastSquareSummary::MEMORY) {
      source.setIndexedProperty<SpreadModel>(frame_index,
          std::numeric_limits<SeFloat>::quiet_NaN(), std::numeric_limits<SeFloat>::quiet_NaN());
      return;
    }

    // Snapshot the fitted position in exactly the same pixel convention as the point fit.
    auto fitted_point = source_model->createPointModelForFrame(reference_coordinates, frame_coordinates, offset);
    const auto flux = source_model->flux->getValue();
    if (!finite(flux) || flux <= 0 || !centre_in_frame(fitted_point)) {
      source.setIndexedProperty<SpreadModel>(frame_index,
          std::numeric_limits<SeFloat>::quiet_NaN(), std::numeric_limits<SeFloat>::quiet_NaN());
      return;
    }
    const auto stamps = SpreadModelUtils::renderModels(fitted_point.getX(), fitted_point.getY(), flux, fwhm,
                                                      width, height, psf);
    const auto result = SpreadModelUtils::computeSpread(*stamps.first, *stamps.second, *image, variances, valid);
    source.setIndexedProperty<SpreadModel>(frame_index, result.getSpreadModel(), result.getSpreadModelError());
  }
}

} // namespace SourceXtractor
