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
#include "SpreadModelAltFit.h"
#include "SEImplementation/Plugin/SpreadModelAlt/JointSpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/JointSpreadModelAltTask.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "ModelFitting/Engine/ChiSquareComparator.h"
#include "ModelFitting/Engine/WorldValueResidual.h"
#include "ModelFitting/Parameters/NeutralConverter.h"
#include "ModelFitting/Models/CompactExponentialModel.h"
#include "ModelFitting/Parameters/ManualParameter.h"

namespace SourceXtractor {

using namespace ModelFitting;
using namespace SpreadModelAltFit;
using Euclid::make_unique;

namespace {

using JointImage = std::shared_ptr<VectorImage<SeFloat>>;
using JointFrameModel = FrameModel<DownSampledImagePsf, JointImage>;

struct JointFrame {
  std::size_t config_index;
  PixelCoordinate offset;
  const MeasurementFrameInfo* info;
  std::shared_ptr<CoordinateSystem> coordinates;
  std::tuple<double, double, double, double> jacobian;
  std::shared_ptr<VectorImage<SeFloat>> kernel;
  double sampling;
  bool renormalize;
  FitStamp stamp;
};

struct JointSolution {
  SourceModel model;
  std::vector<std::shared_ptr<EngineParameter>> fluxes;
  std::shared_ptr<EngineParameter> radius;
  std::vector<std::pair<std::shared_ptr<EngineParameter>, std::shared_ptr<EngineParameter>>> offsets;
  double prior_penalty = 0;
  double objective() const { return chi_squared + prior_penalty; }
  double chi_squared = std::numeric_limits<double>::quiet_NaN();
  double radius_error = std::numeric_limits<double>::quiet_NaN();
  int flags = 0;

  JointSolution(double flux, double x, double y) : model(flux, x, y, 5.0) {}
};

struct JointPosition {
  std::shared_ptr<BasicParameter> x, y;
};

JointPosition framePosition(const JointSolution& fit, const JointFrame& frame, std::size_t index,
                             const std::shared_ptr<CoordinateSystem>& reference_coordinates) {
  std::shared_ptr<BasicParameter> dx = std::make_shared<ManualParameter>(0);
  std::shared_ptr<BasicParameter> dy = dx;
  if (!fit.offsets.empty()) {
    dx = fit.offsets[index].first;
    dy = fit.offsets[index].second;
  }
  auto px = createDependentParameter(
      [coordinates = frame.coordinates, offset = frame.offset, reference_coordinates]
      (double x, double y, double delta_x, double delta_y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld({x + delta_x, y + delta_y})).m_x
            - offset.m_x + 0.5;
      }, fit.model.x, fit.model.y, dx, dy);
  auto py = createDependentParameter(
      [coordinates = frame.coordinates, offset = frame.offset, reference_coordinates]
      (double x, double y, double delta_x, double delta_y) {
        return coordinates->worldToImage(reference_coordinates->imageToWorld({x + delta_x, y + delta_y})).m_y
            - offset.m_y + 0.5;
      }, fit.model.x, fit.model.y, dx, dy);
  return {px, py};
}

// Store both hypotheses' diagnostics in fixed-size arrays, even when coverage differs by source.
void storeOffsets(const JointSolution& fit, const std::vector<JointFrame>& frames,
                   std::vector<double>& dx, std::vector<double>& dy, double& rms) {
  double squared_offset = 0;
  for (std::size_t i = 0; i < frames.size(); ++i) {
    const double x = fit.offsets.empty() ? 0 : fit.offsets[i].first->getValue();
    const double y = fit.offsets.empty() ? 0 : fit.offsets[i].second->getValue();
    dx[frames[i].config_index] = x;
    dy[frames[i].config_index] = y;
    squared_offset += x * x + y * y;
  }
  rms = std::sqrt(squared_offset / frames.size());
}

void solveJoint(JointSolution& fit, const std::vector<JointFrame>& frames,
                const std::shared_ptr<CoordinateSystem>& reference_coordinates,
                double down_scaling, const std::string& engine_name, unsigned int max_iterations,
                double position_prior_sigma) {
  EngineParameterManager manager;
  manager.registerParameter(fit.model.dx);
  manager.registerParameter(fit.model.dy);
  for (const auto& flux : fit.fluxes) {
    manager.registerParameter(flux);
  }
  for (const auto& offset : fit.offsets) {
    manager.registerParameter(offset.first);
    manager.registerParameter(offset.second);
  }
  if (fit.radius) {
    manager.registerParameter(fit.radius);
  }
  ResidualEstimator residuals;
  for (std::size_t i = 0; i < frames.size(); ++i) {
    const auto& frame = frames[i];
    const auto position = framePosition(fit, frame, i, reference_coordinates);
    std::vector<PointModel> points;
    std::vector<std::shared_ptr<ExtendedModel<JointImage>>> extended;
    const int width = frame.stamp.image->getWidth();
    const int height = frame.stamp.image->getHeight();
    if (fit.radius) {
      // Half-light radius and normalization match FlexibleModelFitting's exponential.
      auto i0 = createDependentParameter([](double flux, double radius) {
        return flux / (2 * M_PI * 0.35513 * radius * radius);
      }, fit.fluxes[i], fit.radius);
      auto k = createDependentParameter([](double radius) { return 1.678 / radius; }, fit.radius);
      auto one = std::make_shared<ManualParameter>(1);
      auto zero = std::make_shared<ManualParameter>(0);
      const int size = std::max(4.0, 1.2 * std::max(width, height));
      extended.emplace_back(std::make_shared<CompactExponentialModel<JointImage>>(
          2.0, i0, k, one, one, zero, size, size, position.x, position.y, fit.fluxes[i], frame.jacobian));
    }
    else {
      points.emplace_back(position.x, position.y, fit.fluxes[i]);
    }
    DownSampledImagePsf psf(frame.sampling, frame.kernel, down_scaling, frame.renormalize);
    JointFrameModel model(1.0, width, height, {}, std::move(points), std::move(extended), psf);
    auto image = frame.stamp.image;
    auto weight = frame.stamp.weight;
    residuals.registerBlockProvider(createDataVsModelResiduals(
        image, std::move(model), weight, ChiSquareComparator{}));
  }
  const auto pixel_residual_count = residuals.numberOfResiduals();
  for (const auto& offset : fit.offsets) {
    residuals.registerBlockProvider(make_unique<WorldValueResidual>(offset.first, 0, 1 / position_prior_sigma));
    residuals.registerBlockProvider(make_unique<WorldValueResidual>(offset.second, 0, 1 / position_prior_sigma));
  }
  const auto solution = LeastSquareEngineManager::create(engine_name, max_iterations)->solveProblem(manager, residuals);
  if (solution.status_flag == LeastSquareSummary::ERROR || solution.status_flag == LeastSquareSummary::MEMORY) {
    fit.flags |= JointSpreadModelAlt::ERROR;
    return;
  }
  if (solution.status_flag == LeastSquareSummary::MAX_ITER) {
    fit.flags |= JointSpreadModelAlt::MAX_ITER;
  }
  for (std::size_t i = 0; i < frames.size(); ++i) {
    const auto& frame = frames[i];
    const auto position = framePosition(fit, frame, i, reference_coordinates);
    const PointModel point(position.x, position.y, fit.fluxes[i]);
    if (!centreInFrame(point, frame.offset, *frame.info)) {
      fit.flags |= JointSpreadModelAlt::OUTSIDE;
      return;
    }
    if (!finite(fit.fluxes[i]->getValue()) || fit.fluxes[i]->getValue() <= 0) {
      fit.flags |= JointSpreadModelAlt::ERROR;
      return;
    }
  }
  std::vector<double> values(residuals.numberOfResiduals());
  residuals.populateResiduals(values.begin());
  double chi_squared = 0;
  double prior_penalty = 0;
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i < pixel_residual_count) {
      chi_squared += values[i] * values[i];
    }
    else {
      prior_penalty += values[i] * values[i];
    }
  }
  if (!finite(chi_squared) || !finite(prior_penalty) || !finite(chi_squared + prior_penalty)) {
    fit.flags |= JointSpreadModelAlt::ERROR;
    return;
  }
  fit.chi_squared = chi_squared;
  fit.prior_penalty = prior_penalty;
  // Engines return sigmas in physical parameter units, including converter derivatives.
  if (fit.radius && solution.parameter_sigmas.size() == manager.numberOfParameters()) {
    const double sigma = solution.parameter_sigmas.back();
    if (finite(sigma) && sigma > 0) {
      fit.radius_error = sigma;
    }
  }
}

} // namespace

void JointSpreadModelAltTask::computeProperties(SourceInterface& source) const {
  JointSpreadModelAlt::Result result;
  result.frame_ids = m_frames;
  const double nan = std::numeric_limits<double>::quiet_NaN();
  result.point_offset_x.assign(m_frames.size(), nan);
  result.point_offset_y.assign(m_frames.size(), nan);
  result.extended_offset_x.assign(m_frames.size(), nan);
  result.extended_offset_y.assign(m_frames.size(), nan);
  auto publish = [&]() { source.setProperty<JointSpreadModelAlt>(result); };
  if (m_frames.empty()) {
    result.flags = JointSpreadModelAlt::INSUFFICIENT_DATA;
    publish();
    return;
  }
  const auto& centroid = source.getProperty<PixelCentroid>();
  const double guess_x = centroid.getCentroidX();
  const double guess_y = centroid.getCentroidY();
  const double iso_flux = source.getProperty<IsophotalFlux>().getFlux();
  if (!finite(guess_x) || !finite(guess_y) || !finite(iso_flux)) {
    result.flags = JointSpreadModelAlt::ERROR;
    publish();
    return;
  }
  const auto reference_coordinates = source.getProperty<ReferenceCoordinates>().getCoordinateSystem();
  std::vector<JointFrame> frames;
  double fit_size = 0;
  double max_radius = 2;
  for (std::size_t config_index = 0; config_index < m_frames.size(); ++config_index) {
    const int id = m_frames[config_index];
    const auto& rectangle = source.getProperty<MeasurementFrameRectangle>(id);
    if (!rectangle.isValid() || rectangle.isEmpty()) {
      continue;
    }
    const auto coordinates = source.getProperty<MeasurementFrameCoordinates>(id).getCoordinateSystem();
    const auto& info = source.getProperty<MeasurementFrameInfo>(id);
    const auto centre = coordinates->worldToImage(reference_coordinates->imageToWorld({guess_x, guess_y}));
    // WCS centres are integers; full-frame pixel edges lie at -0.5 and size-0.5.
    if (!finite(centre.m_x) || !finite(centre.m_y) || centre.m_x < -0.5 || centre.m_y < -0.5 ||
        centre.m_x >= info.getWidth() - 0.5 || centre.m_y >= info.getHeight() - 0.5) {
      continue;
    }
    const auto& psf = source.getProperty<SourcePsfProperty>(id);
    if (!psf.getPsf() || !finite(psf.getPixelSampling()) || psf.getPixelSampling() <= 0 ||
        !finite(SpreadModelAltUtils::computePsfFwhm(*psf.getPsf(), psf.getPixelSampling()))) {
      continue;
    }
    const auto jacobian = source.getProperty<JacobianSource>(id).asTuple();
    const double a = std::get<0>(jacobian), b = std::get<1>(jacobian);
    const double c = std::get<2>(jacobian), d = std::get<3>(jacobian);
    const double determinant = a * d - b * c;
    if (!finite(a) || !finite(b) || !finite(c) || !finite(d) || !finite(determinant) || determinant == 0) {
      continue;
    }
    const PixelCoordinate offset(rectangle.getTopLeft().m_x, rectangle.getTopLeft().m_y);
    auto stamp = prepareFitStamp(source.getProperty<MeasurementFrameImages>(id), info,
                                 offset, rectangle.getWidth(), rectangle.getHeight());
    if (stamp.valid_pixels < 2) {
      continue;
    }
    result.n_pixels += stamp.valid_pixels;
    fit_size = std::max(fit_size, rectangle.getWidth() * double(rectangle.getHeight()) /
                                    (psf.getPixelSampling() * psf.getPixelSampling()));
    max_radius = std::max(max_radius, 0.5 * std::max(rectangle.getWidth(), rectangle.getHeight()) /
                                         std::sqrt(std::abs(determinant)));
    frames.push_back({config_index, offset, &info, coordinates, jacobian, psf.getPsf(), psf.getPixelSampling(),
                      m_renormalize.at(id), std::move(stamp)});
  }
  result.n_frames = frames.size();
  // There are N+3 shared-position parameters plus 2N offset parameters when enabled.
  // Their 2N Gaussian residuals restore identifiability, so the conservative data
  // requirement stays Npix > N+3. Priors never count as usable image pixels.
  if (frames.empty() || result.n_pixels <= frames.size() + 3) {
    result.flags = JointSpreadModelAlt::INSUFFICIENT_DATA;
    publish();
    return;
  }
  double down_scaling = m_scale_factor;
  if (fit_size > 2 * m_max_fit_area) {
    down_scaling *= std::sqrt(m_max_fit_area / fit_size);
  }
  const double flux_guess = std::max(iso_flux, 0.0001);
  JointSolution point(flux_guess, guess_x, guess_y);
  for (std::size_t i = 0; i < frames.size(); ++i) {
    point.fluxes.push_back(std::make_shared<EngineParameter>(flux_guess,
        make_unique<ExpSigmoidConverter>(flux_guess * 0.00001, flux_guess * 1000)));
  }
  if (m_position_prior_sigma > 0) {
    for (std::size_t i = 0; i < frames.size(); ++i) {
      point.offsets.emplace_back(std::make_shared<EngineParameter>(0, make_unique<NeutralConverter>()),
                                 std::make_shared<EngineParameter>(0, make_unique<NeutralConverter>()));
    }
  }
  solveJoint(point, frames, reference_coordinates, down_scaling, m_engine, m_max_iterations, m_position_prior_sigma);
  result.flags = point.flags;
  if (!finite(point.chi_squared)) {
    publish();
    return;
  }
  result.point_chi_squared = point.chi_squared;
  result.point_prior_penalty = point.prior_penalty;
  storeOffsets(point, frames, result.point_offset_x, result.point_offset_y, result.point_offset_rms);
  // Two starts reduce sensitivity to a flat small-radius response. The explicit
  // point solution is also an extended-model candidate at the zero-radius limit.
  double best_objective = point.objective();
  result.extended_chi_squared = point.chi_squared;
  result.extended_prior_penalty = point.prior_penalty;
  storeOffsets(point, frames, result.extended_offset_x, result.extended_offset_y, result.extended_offset_rms);
  result.radius = 0;
  result.flags |= JointSpreadModelAlt::BOUNDARY;
  bool extended_solved = false;
  int failure_flags = 0;
  int extended_flags = 0;
  for (double start : {0.3, 1.5}) {
    JointSolution extended(flux_guess, guess_x, guess_y);
    extended.model.dx->setValue(point.model.dx->getValue());
    extended.model.dy->setValue(point.model.dy->getValue());
    for (const auto& flux : point.fluxes) {
      extended.fluxes.push_back(std::make_shared<EngineParameter>(flux->getValue(),
          make_unique<ExpSigmoidConverter>(flux_guess * 0.00001, flux_guess * 1000)));
    }
    for (const auto& offset : point.offsets) {
      extended.offsets.emplace_back(
          std::make_shared<EngineParameter>(offset.first->getValue(), make_unique<NeutralConverter>()),
          std::make_shared<EngineParameter>(offset.second->getValue(), make_unique<NeutralConverter>()));
    }
    // Positive lower bound protects the exponential renderer's 1/r and 1/r^2.
    extended.radius = std::make_shared<EngineParameter>(std::min(start, max_radius * 0.8),
        make_unique<ExpSigmoidConverter>(0.01, max_radius));
    solveJoint(extended, frames, reference_coordinates, down_scaling, m_engine, m_max_iterations,
               m_position_prior_sigma);
    if (!finite(extended.chi_squared)) {
      failure_flags |= extended.flags;
      continue;
    }
    extended_solved = true;
    extended_flags |= extended.flags;
    if (extended.objective() < best_objective) {
      best_objective = extended.objective();
      result.extended_chi_squared = extended.chi_squared;
      result.extended_prior_penalty = extended.prior_penalty;
      storeOffsets(extended, frames, result.extended_offset_x, result.extended_offset_y, result.extended_offset_rms);
      result.radius = extended.radius->getValue();
      result.radius_error = extended.radius_error;
      result.flags = point.flags | extended.flags;
      if (result.radius <= 0.02 || result.radius >= 0.99 * max_radius) {
        result.flags |= JointSpreadModelAlt::BOUNDARY;
        result.radius_error = std::numeric_limits<double>::quiet_NaN();
      }
    }
  }
  if (!extended_solved) {
    result.radius = nan;
    result.extended_chi_squared = nan;
    result.extended_prior_penalty = nan;
    result.extended_offset_rms = nan;
    result.extended_offset_x.assign(m_frames.size(), nan);
    result.extended_offset_y.assign(m_frames.size(), nan);
    result.flags = point.flags | failure_flags | JointSpreadModelAlt::ERROR;
    publish();
    return;
  }
  if (result.radius == 0) {
    result.flags |= extended_flags;
  }
  result.delta_chi_squared = point.chi_squared - result.extended_chi_squared;
  result.delta_objective = point.objective() - best_objective;
  publish();
}

} // namespace SourceXtractor
