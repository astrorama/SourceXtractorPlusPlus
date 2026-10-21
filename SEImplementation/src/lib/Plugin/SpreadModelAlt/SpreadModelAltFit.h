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
#ifndef _SEIMPLEMENTATION_SPREADMODELALT_FIT_H_
#define _SEIMPLEMENTATION_SPREADMODELALT_FIT_H_

#include <algorithm>
#include <cmath>
#include <limits>

#include "AlexandriaKernel/memory_tools.h"

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

#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltUtils.h"
#include "SEUtils/IsNan.h"

#include "SEImplementation/Image/DownSampledImagePsf.h"

#include "SEImplementation/Plugin/SourcePsf/SourcePsfProperty.h"

#include "ModelFitting/Engine/AsinhChiSquareComparator.h"

namespace SourceXtractor {
namespace SpreadModelAltFit {

using namespace ModelFitting;
using Euclid::make_unique;

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
      PixelCoordinate offset, std::shared_ptr<BasicParameter> amplitude = nullptr) {

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

    return PointModel(frame_x, frame_y, amplitude ? amplitude : flux);
  }


};

inline bool finite(double value) {
  return !fastmath_isnan(value) && !fastmath_isinf(value);
}

inline SpreadModelAlt invalidResult() {
  const auto nan = std::numeric_limits<SeFloat>::quiet_NaN();
  return SpreadModelAlt(nan, nan);
}

// Keep the mask and noise variances alongside the fit inputs for the final statistic.
struct FitStamp {
  std::shared_ptr<VectorImage<SeFloat>> image;
  std::shared_ptr<VectorImage<SeFloat>> weight;
  std::vector<double> variances;
  std::vector<bool> valid;
  std::size_t valid_pixels = 0;
};

inline double pixelVariance(SeFloat pixel_value, SeFloat background_variance, const MeasurementFrameInfo& info) {
  const SeFloat gain = info.getGain();
  if (!(finite(pixel_value) && finite(background_variance) && background_variance > 0 &&
        background_variance <= info.getVarianceThreshold())) {
    return 0;
  }
  // Negative background-subtracted values must not subtract shot-noise variance.
  const double variance = double(background_variance) + (gain > 0 ? std::max<double>(pixel_value, 0) / gain : 0);
  return finite(variance) && variance > 0 ? variance : 0;
}

inline FitStamp prepareFitStamp(const MeasurementFrameImages& images, const MeasurementFrameInfo& info,
                         PixelCoordinate offset, std::size_t width, std::size_t height) {
  FitStamp stamp;
  stamp.image = VectorImage<SeFloat>::create(images.getImageChunk(
      LayerSubtractedImage, offset.m_x, offset.m_y, width, height));
  stamp.weight = VectorImage<SeFloat>::create(width, height);
  stamp.variances.assign(width * height, 0);
  stamp.valid.assign(width * height, false);
  const auto frame_image = images.getLockedImage(LayerSubtractedImage);
  const auto original_image = images.getLockedImage(LayerOriginalImage);
  const auto saturation = info.getSaturation();
  const auto variance_map = images.getLockedImage(LayerVarianceMap);
  // Share the pixel mask between fitting and spread calculation. The fitter takes
  // residual weights 1/sqrt(V); the statistic uses binary weights and keeps V separately.
  for (std::size_t y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < width; ++x) {
      const auto i = y * width + x;
      // The saturation threshold applies to original-image values, before background subtraction.
      const bool saturated = saturation > 0 &&
          original_image->getValue(offset.m_x + x, offset.m_y + y) >= saturation;
      const double variance = saturated ? 0 : pixelVariance(
          frame_image->getValue(offset.m_x + x, offset.m_y + y),
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

inline bool centreInFrame(const PointModel& point, PixelCoordinate offset, const MeasurementFrameInfo& info) {
  // Model coordinates have pixel centres at half integers. Restore the
  // full-frame coordinates and test against pixel edges, not stamp edges.
  const double x = point.getX() + offset.m_x;
  const double y = point.getY() + offset.m_y;
  return finite(x) && finite(y) && x >= 0 && y >= 0 && x < info.getWidth() && y < info.getHeight();
}

inline bool fitPointSource(SourceModel& source_model, PointModel point, FitStamp& stamp,
                    const DownSampledImagePsf& psf, const std::string& engine_name, unsigned int max_iterations) {
  using VectorImageType = std::shared_ptr<VectorImage<SeFloat>>;
  std::vector<PointModel> point_models;
  point_models.emplace_back(std::move(point));
  FrameModel<DownSampledImagePsf, VectorImageType> frame_model{
      1.0, std::size_t(stamp.image->getWidth()), std::size_t(stamp.image->getHeight()),
      {}, std::move(point_models), {}, psf};
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

} // namespace SpreadModelAltFit
} // namespace SourceXtractor

#endif
