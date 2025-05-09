/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/AsinhChiSquareComparator.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"
#include "ModelFitting/Engine/LeastSquareEngineManager.h"

#include "SEImplementation/Image/ImagePsf.h"
#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"

#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinates.h"
#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"

#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/SourcePsf/SourcePsfProperty.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/WorldCentroid/WorldCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFitting.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.h"

namespace SourceXtractor {

using namespace ModelFitting;

static auto logger = Elements::Logging::getLogger("FlexibleModelFitting");

FlexibleModelFittingIterativeTask::FlexibleModelFittingIterativeTask(const std::string &least_squares_engine,
    unsigned int max_iterations, double modified_chi_squared_scale,
    std::vector<std::shared_ptr<FlexibleModelFittingParameter>> parameters,
    std::vector<std::shared_ptr<FlexibleModelFittingFrame>> frames,
    std::vector<std::shared_ptr<FlexibleModelFittingPrior>> priors,
    std::vector<bool> should_renormalize,
    double scale_factor,
    int meta_iterations,
    double deblend_factor,
    double meta_iteration_stop,
    size_t max_fit_size,
    WindowType window_type,
    double ellipse_scale
    )
    : m_least_squares_engine(least_squares_engine), m_max_iterations(max_iterations),
      m_modified_chi_squared_scale(modified_chi_squared_scale), m_scale_factor(scale_factor),
      m_meta_iterations(meta_iterations), m_deblend_factor(deblend_factor), m_meta_iteration_stop(meta_iteration_stop),
      m_max_fit_size(max_fit_size * max_fit_size), m_parameters(parameters), m_frames(frames), m_priors(priors),
      m_should_renormalize(should_renormalize),
      m_window_type(window_type), m_ellipse_scale(ellipse_scale) {}

FlexibleModelFittingIterativeTask::~FlexibleModelFittingIterativeTask() {
}

PixelRectangle FlexibleModelFittingIterativeTask::getUnclippedFittingRect(SourceInterface& source, int frame_index) const {
  auto fitting_rect = source.getProperty<MeasurementFrameRectangle>(frame_index).getRect();

  if (m_window_type == WindowType::ROTATED_ELLIPSE) {
    auto ellipse = getFittingEllipse(source, frame_index);

    ellipse.m_a *= m_ellipse_scale;
    ellipse.m_b *= m_ellipse_scale;
    return getEllipseRect(ellipse);
  }

  if (fitting_rect.getWidth() <= 0 || fitting_rect.getHeight() <= 0) {
    return PixelRectangle();
  } else {
    auto min = fitting_rect.getTopLeft();
    auto max = fitting_rect.getBottomRight();

    // FIXME temporary, for now just enlarge the area by a fixed amount of pixels
    PixelCoordinate border = (max - min) * .8 + PixelCoordinate(2, 2);

    min -= border;
    max += border;

    if (m_window_type == WindowType::DISK_MIN || m_window_type == WindowType::SQUARE_MIN) {
      if (max.m_x - min.m_x > max.m_y - min.m_y) {
        min.m_x += ((max.m_x - min.m_x) - (max.m_y - min.m_y)) / 2;
        max.m_x = min.m_x + (max.m_y - min.m_y);
      } else {
        min.m_y += ((max.m_y - min.m_y) - (max.m_x - min.m_x)) / 2;
        max.m_y = min.m_y + (max.m_x - min.m_x);
      }
    }

    if (m_window_type == WindowType::DISK_MAX || m_window_type == WindowType::SQUARE_MAX) {
      if (max.m_x - min.m_x < max.m_y - min.m_y) {
        min.m_x += ((max.m_x - min.m_x) - (max.m_y - min.m_y)) / 2;
        max.m_x = min.m_x + (max.m_y - min.m_y);
      } else {
        min.m_y += ((max.m_y - min.m_y) - (max.m_x - min.m_x)) / 2;
        max.m_y = min.m_y + (max.m_x - min.m_x);
      }
    }

    if (m_window_type == WindowType::DISK_AREA || m_window_type == WindowType::SQUARE_AREA) {
      int area = (max.m_x - min.m_x) * (max.m_y - min.m_y);
      int size = int(sqrt(area));
      min.m_x += ((max.m_x - min.m_x) - size) / 2;
      max.m_x = min.m_x + size;
      min.m_y += ((max.m_y - min.m_y) - size) / 2;
      max.m_y = min.m_y + size;
    }

    return PixelRectangle(min, max);
  }
}

PixelRectangle FlexibleModelFittingIterativeTask::clipFittingRect(PixelRectangle fitting_rect,
    SourceInterface& source, int frame_index) const {
  const auto& frame_info = source.getProperty<MeasurementFrameInfo>(frame_index);

  auto min = fitting_rect.getTopLeft();
  auto max = fitting_rect.getBottomRight();

  // clip to image size
  min.m_x = std::max(min.m_x, 0);
  min.m_y = std::max(min.m_y, 0);
  max.m_x = std::min(max.m_x, frame_info.getWidth() - 1);
  max.m_y = std::min(max.m_y, frame_info.getHeight() - 1);

  return PixelRectangle(min, max);
}

PixelRectangle FlexibleModelFittingIterativeTask::getFittingRect(SourceInterface& source, int frame_index) const {
  return clipFittingRect(getUnclippedFittingRect(source, frame_index), source, frame_index);
}

FlexibleModelFittingIterativeTask::FittingEllipse FlexibleModelFittingIterativeTask::getFittingEllipse(SourceInterface& source, int frame_index) const {
  auto source_shape = source.getProperty<ShapeParameters>();
  auto centroid = source.getProperty<PixelCentroid>();

  // Ellipse in detection frame
  auto detect_a = source_shape.getEllipseA();
  auto detect_b = source_shape.getEllipseB();
  auto detect_theta = source_shape.getEllipseTheta();

  FittingEllipse ellipse;

  ellipse.m_x = centroid.getCentroidX();
  ellipse.m_y = centroid.getCentroidY();

  ellipse.m_a = detect_a;
  ellipse.m_b = detect_b;
  ellipse.m_theta = detect_theta;

  return transformEllipse(ellipse, source, frame_index);
}

PixelRectangle FlexibleModelFittingIterativeTask::getEllipseRect(FittingEllipse ellipse) const {
  double cos_theta = std::cos(ellipse.m_theta);
  double sin_theta = std::sin(ellipse.m_theta);

  // horizontal and vertical components for the a and b axes of ellipses
  double a_cos = ellipse.m_a * std::abs(cos_theta);
  double a_sin = ellipse.m_a * std::abs(sin_theta);
  double b_sin = ellipse.m_b * std::abs(sin_theta);
  double b_cos = ellipse.m_b * std::abs(cos_theta);

  double half_width = std::sqrt(a_cos*a_cos + b_sin*b_sin) + 1;
  double half_height = std::sqrt(a_sin*a_sin + b_cos*b_cos) + 1;

  PixelCoordinate min_corner, max_corner;
  min_corner.m_x = ellipse.m_x - half_width;
  min_corner.m_y = ellipse.m_y - half_height;
  max_corner.m_x = ellipse.m_x + half_width;
  max_corner.m_y = ellipse.m_y + half_height;

  return {min_corner, max_corner};
}

FlexibleModelFittingIterativeTask::FittingEllipse FlexibleModelFittingIterativeTask::transformEllipse(
    FittingEllipse ellipse, SourceInterface& source, int frame_index) const {
  auto frame_coordinates = source.getProperty<MeasurementFrameCoordinates>(frame_index).getCoordinateSystem();
  auto ref_coordinates = source.getProperty<ReferenceCoordinates>().getCoordinateSystem();
  auto jacobian = source.getProperty<JacobianSource>(frame_index).asTuple();

  auto coord = frame_coordinates->worldToImage(
      ref_coordinates->imageToWorld(ImageCoordinate(ellipse.m_x, ellipse.m_y)));

  double cos_theta = std::cos(ellipse.m_theta);
  double sin_theta = std::sin(ellipse.m_theta);

  double x_basis_x = std::get<0>(jacobian);
  double x_basis_y = std::get<1>(jacobian);
  double y_basis_x = std::get<2>(jacobian);
  double y_basis_y = std::get<3>(jacobian);

  // get vectors for the axis
  double major_x = ellipse.m_a * cos_theta;
  double major_y = ellipse.m_a * sin_theta;
  double minor_x = -ellipse.m_b * sin_theta;
  double minor_y = ellipse.m_b * cos_theta;

  // Project major axis into the new coordinate system
  double new_major_x = major_x * x_basis_x + major_y * y_basis_x;
  double new_major_y = major_x * x_basis_y + major_y * y_basis_y;

  // Project minor axis into the new coordinate system
  double new_minor_x = minor_x * x_basis_x + minor_y * y_basis_x;
  double new_minor_y = minor_x * x_basis_y + minor_y * y_basis_y;

  // Calculate new semi-major and semi-minor lengths
  double new_a = std::sqrt(new_major_x * new_major_x + new_major_y * new_major_y);
  double new_b = std::sqrt(new_minor_x * new_minor_x + new_minor_y * new_minor_y);

  // Calculate new angle (atan2 handles angle wrapping automatically)
  double new_theta = std::atan2(new_major_y, new_major_x);

  return {coord.m_x, coord.m_y, new_a, new_b, new_theta};
}

bool FlexibleModelFittingIterativeTask::isFrameValid(SourceInterface& source, int frame_index) const {
  auto stamp_rect = getFittingRect(source, frame_index);
  return stamp_rect.getWidth() > 0 && stamp_rect.getHeight() > 0;
}

std::shared_ptr<VectorImage<SeFloat>> FlexibleModelFittingIterativeTask::createImageCopy(
    SourceInterface& source, int frame_index) const {
  const auto& frame_images = source.getProperty<MeasurementFrameImages>(frame_index);
  auto rect = getFittingRect(source, frame_index);

  auto image = VectorImage<SeFloat>::create(frame_images.getImageChunk(
      LayerSubtractedImage, rect.getTopLeft().m_x, rect.getTopLeft().m_y, rect.getWidth(), rect.getHeight()));

  return image;
}


FrameModel<DownSampledImagePsf, std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>>
FlexibleModelFittingIterativeTask::createFrameModel(
    SourceInterface& source, double pixel_scale, FlexibleModelFittingParameterManager& manager,
    std::shared_ptr<FlexibleModelFittingFrame> frame, PixelRectangle stamp_rect, double down_scaling) const {

  int frame_index = frame->getFrameNb();

  auto frame_coordinates = source.getProperty<MeasurementFrameCoordinates>(frame_index).getCoordinateSystem();
  auto ref_coordinates = source.getProperty<ReferenceCoordinates>().getCoordinateSystem();

  auto psf_property = source.getProperty<SourcePsfProperty>(frame_index);
  auto jacobian = source.getProperty<JacobianSource>(frame_index).asTuple();

  // The model fitting module expects to get a PSF with a pixel scale, but we have the pixel sampling step size
  // It will be used to compute the rastering grid size, and after convolving with the PSF the result will be
  // downscaled before copied into the frame image.
  // We can multiply here then, as the unit is pixel/pixel, rather than "/pixel or similar
  auto source_psf = DownSampledImagePsf(psf_property.getPixelSampling(), psf_property.getPsf(),
      down_scaling, m_should_renormalize[frame_index]);

  std::vector<ConstantModel> constant_models;
  std::vector<PointModel> point_models;
  std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>> extended_models;

  double model_size = std::max(stamp_rect.getWidth(), stamp_rect.getHeight());
  for (auto model : frame->getModels()) {
    model->addForSource(manager, source, constant_models, point_models, extended_models, model_size,
        jacobian, ref_coordinates, frame_coordinates, stamp_rect.getTopLeft());
  }

  // Full frame model with all sources
  FrameModel<DownSampledImagePsf, std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>> frame_model(
    pixel_scale, (size_t) stamp_rect.getWidth(), (size_t) stamp_rect.getHeight(),
    std::move(constant_models), std::move(point_models), std::move(extended_models), source_psf);

  return frame_model;
}

std::shared_ptr<VectorImage<SeFloat>> FlexibleModelFittingIterativeTask::createWeightImage(
    SourceInterface& source, int frame_index) const {
  const auto& frame_images = source.getProperty<MeasurementFrameImages>(frame_index);
  auto frame_coordinates = source.getProperty<MeasurementFrameCoordinates>(frame_index).getCoordinateSystem();
  auto ref_coordinates = source.getProperty<ReferenceCoordinates>().getCoordinateSystem();

  auto frame_image = frame_images.getLockedImage(LayerSubtractedImage);
  auto frame_image_thresholded = frame_images.getLockedImage(LayerThresholdedImage);
  auto variance_map = frame_images.getLockedImage(LayerVarianceMap);

  const auto& frame_info = source.getProperty<MeasurementFrameInfo>(frame_index);
  SeFloat gain = frame_info.getGain();
  SeFloat saturation = frame_info.getSaturation();

  auto unclipped_rect = getUnclippedFittingRect(source, frame_index);
  auto rect = clipFittingRect(unclipped_rect, source, frame_index);
  auto weight = VectorImage<SeFloat>::create(rect.getWidth(), rect.getHeight());

  // Precompute ellipse parameters

  FittingEllipse ellipse;
  auto rad = std::min(unclipped_rect.getWidth() / 2.0, unclipped_rect.getHeight() / 2.0) - 1.0;

  if (m_window_type == WindowType::ROTATED_ELLIPSE) {
    ellipse = getFittingEllipse(source, frame_index);
  } else {
    // we don't want to require the ShapeParameters property when not using ROTATED_ELLIPSE
    auto centroid = source.getProperty<WorldCentroid>();
    auto coord = frame_coordinates->worldToImage(centroid.getCentroid());
    ellipse.m_x = coord.m_x;
    ellipse.m_y = coord.m_y;
    ellipse.m_a = rad;
    ellipse.m_b = rad;
    ellipse.m_theta = 0;
  }

  double cos_theta = std::cos(ellipse.m_theta);
  double sin_theta = std::sin(ellipse.m_theta);

  double a_squared = ellipse.m_a * ellipse.m_a * m_ellipse_scale * m_ellipse_scale;
  double b_squared = ellipse.m_b * ellipse.m_b * m_ellipse_scale * m_ellipse_scale;

  // Precompute components of the ellipse's quadratic form
  double A = (cos_theta * cos_theta) / a_squared + (sin_theta * sin_theta) / b_squared;
  double B = 2 * cos_theta * sin_theta * (1 / a_squared - 1 / b_squared);
  double C = (sin_theta * sin_theta) / a_squared + (cos_theta * cos_theta) / b_squared;

  for (int y = 0; y < rect.getHeight(); y++) {
    for (int x = 0; x < rect.getWidth(); x++) {
      auto back_var = variance_map->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y);
      auto pixel_val = frame_image->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y);

      auto dx = x + rect.getTopLeft().m_x - ellipse.m_x;
      auto dy = y + rect.getTopLeft().m_y - ellipse.m_y;

      if (saturation > 0 && pixel_val > saturation) {
        weight->at(x, y) = 0;
      } else if (gain > 0.0 && pixel_val > 0.0) {
        weight->at(x, y) = sqrt(1.0 / (back_var + pixel_val / gain));
      } else {
        weight->at(x, y) = sqrt(1.0 / back_var); // infinite gain
      }

      if (m_window_type == WindowType::DISK_MIN || m_window_type == WindowType::DISK_MAX
          || m_window_type == WindowType::DISK_AREA) {
        if (dx * dx + dy * dy > rad * rad) {
          weight->at(x, y) = 0;
        }
      } else if (m_window_type == WindowType::ALIGNED_ELLIPSE) {
        auto w = unclipped_rect.getWidth() / 2.0;
        auto h = unclipped_rect.getHeight() / 2.0;
        if (dx / w * dx / w + dy / h * dy / h > 1) {
          weight->at(x, y) = 0;
        }
      } else if (m_window_type == WindowType::ROTATED_ELLIPSE) {
        // Apply the quadratic form of the ellipse equation
        double value = A * dx * dx + B * dx * dy + C * dy * dy;
        if (value > 1.0) {
          weight->at(x, y) = 0;
        }
      }
    }
  }

  return weight;
}

void FlexibleModelFittingIterativeTask::computeProperties(SourceGroupInterface& group) const {
  FittingState fitting_state;

  for (auto& source : group) {
    SourceState initial_state;
    initial_state.flags = Flags::NONE;
    initial_state.iterations = 0;
    initial_state.stop_reason = 0;
    initial_state.reduced_chi_squared = 0.0;
    initial_state.duration = 0.0;

    for (auto parameter : m_parameters) {
      auto free_parameter = std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter);
      if (free_parameter != nullptr) {
        initial_state.parameters_initial_values[free_parameter->getId()] =
            initial_state.parameters_values[free_parameter->getId()] = free_parameter->getInitialValue(source);
      } else {
        initial_state.parameters_initial_values[parameter->getId()] =
            initial_state.parameters_values[parameter->getId()] = 0.0;
      }
      // Make sure we have a default value for sigmas in case we cannot do the fit
      initial_state.parameters_sigmas[parameter->getId()] = std::numeric_limits<double>::quiet_NaN();
    }

    // Store fitting areas for later output
    for (auto frame : m_frames) {
      int frame_index = frame->getFrameNb();
      // Validate that each frame covers the model fitting region
      if (isFrameValid(source, frame_index)) {
        auto stamp_rect = getFittingRect(source, frame_index);
        initial_state.fitting_areas_x.push_back(stamp_rect.getWidth());
        initial_state.fitting_areas_y.push_back(stamp_rect.getHeight());
      } else {
        initial_state.fitting_areas_x.push_back(-1.f);
        initial_state.fitting_areas_y.push_back(-1.f);
      }
    }

    fitting_state.source_states.emplace_back(std::move(initial_state));
  }

  // TODO Sort sources by flux to fit brightest sources first?

  // iterate over the whole group, fitting sources one at a time

  double prev_chi_squared = 999999.9;
  for (int iteration = 0; iteration < m_meta_iterations; iteration++) {
    int index = 0;
    for (auto& source : group) {
      fitSource(group, source, index, fitting_state);
      index++;
    }

    // evaluate reduced chi squared to bail out of meta iterations if no longer improving the fit

    double chi_squared = 0.0;
    for (auto& source_state : fitting_state.source_states) {
      chi_squared += source_state.reduced_chi_squared;
    }
    chi_squared /= fitting_state.source_states.size();

    if (fabs(chi_squared - prev_chi_squared) / chi_squared < m_meta_iteration_stop) {
     break;
    }

    prev_chi_squared = chi_squared;
  }


  // Remove parameters that couldn't be fit from the output

  for (size_t index = 0; index < group.size(); index++){
    auto& source_state = fitting_state.source_states.at(index);

    for (auto parameter : m_parameters) {
      auto free_parameter = std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter);

      if (free_parameter != nullptr && !source_state.parameters_fitted[parameter->getId()]) {
        source_state.parameters_values[parameter->getId()] = std::numeric_limits<double>::quiet_NaN();
        source_state.parameters_sigmas[parameter->getId()] = std::numeric_limits<double>::quiet_NaN();
      }
    }
  }

  // output a property for every source
  size_t index = 0;
  for (auto& source : group) {
    auto& source_state = fitting_state.source_states.at(index);

    int meta_iterations = source_state.chi_squared_per_meta.size();
    source_state.chi_squared_per_meta.resize(m_meta_iterations);
    source_state.iterations_per_meta.resize(m_meta_iterations);

    source.setProperty<FlexibleModelFitting>(source_state.iterations, source_state.stop_reason,
        source_state.reduced_chi_squared, source_state.duration, source_state.flags,
        source_state.parameters_values, source_state.parameters_sigmas,
        source_state.chi_squared_per_meta, source_state.iterations_per_meta,
        meta_iterations, source_state.fitting_areas_x, source_state.fitting_areas_y);

    index++;
  }

  updateCheckImages(group, 1.0, fitting_state);
}


std::shared_ptr<VectorImage<SeFloat>> FlexibleModelFittingIterativeTask::createDeblendImage(
    SourceGroupInterface& group, SourceInterface& source, int source_index,
    std::shared_ptr<FlexibleModelFittingFrame> frame, FittingState& state) const {
  int frame_index = frame->getFrameNb();
  auto rect = getFittingRect(source, frame_index);

  double pixel_scale = 1.0;
  FlexibleModelFittingParameterManager parameter_manager;
  ModelFitting::EngineParameterManager engine_parameter_manager {};
  int n_free_parameters = 0;

  int index = 0;
  for (auto& src : group) {
    if (index != source_index) {
      for (auto parameter : m_parameters) {
        auto free_parameter = std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter);

        if (free_parameter != nullptr) {
          ++n_free_parameters;

          // Initial with the values from the current iteration run
          parameter_manager.addParameter(src, parameter,
              free_parameter->create(parameter_manager, engine_parameter_manager, src,
                  state.source_states[index].parameters_initial_values.at(free_parameter->getId()),
                  state.source_states[index].parameters_values.at(free_parameter->getId())));
        } else {
          parameter_manager.addParameter(src, parameter,
              parameter->create(parameter_manager, engine_parameter_manager, src));
        }
      }
    }
    index++;
  }

  auto deblend_image = VectorImage<SeFloat>::create(rect.getWidth(), rect.getHeight());
  index = 0;
  for (auto& src : group) {
    if (index != source_index) {
        auto frame_model = createFrameModel(src, pixel_scale, parameter_manager, frame, rect);
        auto final_stamp = frame_model.getImage();

        for (int y = 0; y < final_stamp->getHeight(); ++y) {
          for (int x = 0; x < final_stamp->getWidth(); ++x) {
            deblend_image->at(x, y) += final_stamp->at(x, y);
          }
        }
    }
    index++;
  }

  return deblend_image;
}

int FlexibleModelFittingIterativeTask::fitSourcePrepareParameters(
                                                    FlexibleModelFittingParameterManager& parameter_manager,
                                                    ModelFitting::EngineParameterManager& engine_parameter_manager,
                                                    SourceInterface& source, int index, FittingState& state) const {
  int free_parameters_nb = 0;
  for (auto parameter : m_parameters) {
    auto free_parameter = std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter);

    if (free_parameter != nullptr) {
      ++free_parameters_nb;

      // Initial with the values from the current iteration run
      parameter_manager.addParameter(source, parameter,
          free_parameter->create(parameter_manager, engine_parameter_manager, source,
              state.source_states[index].parameters_initial_values.at(free_parameter->getId()),
              state.source_states[index].parameters_values.at(free_parameter->getId())));
    } else {
      parameter_manager.addParameter(source, parameter,
          parameter->create(parameter_manager, engine_parameter_manager, source));
    }

  }

  // Reset access checks, as a dependent parameter could have triggered it
  parameter_manager.clearAccessCheck();

  return free_parameters_nb;
}

int FlexibleModelFittingIterativeTask::fitSourcePrepareModels(FlexibleModelFittingParameterManager& parameter_manager,
    ResidualEstimator& res_estimator, int& good_pixels,
    SourceGroupInterface& group, SourceInterface& source, int index, FittingState& state, double down_scaling) const {

  double pixel_scale = 1.0;

  int valid_frames = 0;
  for (auto frame : m_frames) {
    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(source, frame_index)) {
      valid_frames++;

      auto stamp_rect = getFittingRect(source, frame_index);
      auto frame_model = createFrameModel(source, pixel_scale, parameter_manager, frame, stamp_rect, down_scaling);

      auto image = createImageCopy(source, frame_index);

      auto deblend_image = createDeblendImage(group, source, index, frame, state);
      for (int y = 0; y < image->getHeight(); ++y) {
        for (int x = 0; x < image->getWidth(); ++x) {
          image->at(x, y) -= m_deblend_factor * deblend_image->at(x, y);
        }
      }

      auto weight = createWeightImage(source, frame_index);

      // count number of pixels that can be used for fitting
      for (int y = 0; y < weight->getHeight(); ++y) {
        for (int x = 0; x < weight->getWidth(); ++x) {
          good_pixels += (weight->at(x, y) != 0.);
        }
      }

      // Setup residuals
      auto data_vs_model =
        createDataVsModelResiduals(image, std::move(frame_model), weight,
                                   //LogChiSquareComparator(m_modified_chi_squared_scale));
                                   AsinhChiSquareComparator(m_modified_chi_squared_scale));
      res_estimator.registerBlockProvider(std::move(data_vs_model));
    }
  }

  return valid_frames;
}

SeFloat FlexibleModelFittingIterativeTask::fitSourceComputeChiSquared(FlexibleModelFittingParameterManager& parameter_manager,
    SourceGroupInterface& group, SourceInterface& source, int index, FittingState& state) const {

  double pixel_scale = 1.0;

  int total_data_points = 0;
  SeFloat avg_reduced_chi_squared = computeChiSquared(group, source, index,pixel_scale, parameter_manager, total_data_points, state);

  int nb_of_free_parameters = 0;
  for (auto parameter : m_parameters) {
    bool is_free_parameter = std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter).get();
    bool accessed_by_modelfitting = parameter_manager.isParamAccessed(source, parameter);
    if (is_free_parameter && accessed_by_modelfitting) {
      nb_of_free_parameters++;
    }
  }
  avg_reduced_chi_squared /= (total_data_points - nb_of_free_parameters);

  return avg_reduced_chi_squared;
}

void FlexibleModelFittingIterativeTask::fitSourceUpdateState(
    FlexibleModelFittingParameterManager& parameter_manager, SourceInterface& source,
    SeFloat avg_reduced_chi_squared, SeFloat duration, unsigned int iterations, unsigned int stop_reason, Flags flags,
    ModelFitting::LeastSquareSummary solution,
    int index, FittingState& state) const {
  ////////////////////////////////////////////////////////////////////////////////////
  // Collect parameters for output
  std::unordered_map<int, double> parameters_values, parameters_sigmas;
  std::unordered_map<int, bool> parameters_fitted;

  for (auto parameter : m_parameters) {
    bool is_dependent_parameter = std::dynamic_pointer_cast<FlexibleModelFittingDependentParameter>(parameter).get();
    bool is_constant_parameter = std::dynamic_pointer_cast<FlexibleModelFittingConstantParameter>(parameter).get();
    bool accessed_by_modelfitting = parameter_manager.isParamAccessed(source, parameter);
    auto modelfitting_parameter = parameter_manager.getParameter(source, parameter);

    if (is_constant_parameter || is_dependent_parameter || accessed_by_modelfitting) {
      parameters_values[parameter->getId()] = modelfitting_parameter->getValue();
      parameters_sigmas[parameter->getId()] = parameter->getSigma(parameter_manager, source, solution.parameter_sigmas);
      parameters_fitted[parameter->getId()] = true;
    } else {
      parameters_values[parameter->getId()] = state.source_states[index].parameters_values[parameter->getId()];
      parameters_sigmas[parameter->getId()] = state.source_states[index].parameters_sigmas[parameter->getId()];
      parameters_fitted[parameter->getId()] = false;

      // Need to cascade the NaN to any potential dependent parameter
      auto engine_parameter = std::dynamic_pointer_cast<EngineParameter>(modelfitting_parameter);
      if (engine_parameter) {
        engine_parameter->setEngineValue(std::numeric_limits<double>::quiet_NaN());
      }

      flags |= Flags::PARTIAL_FIT;
    }
  }

  state.source_states[index].parameters_values = parameters_values;
  state.source_states[index].parameters_sigmas = parameters_sigmas;
  state.source_states[index].parameters_fitted = parameters_fitted;
  state.source_states[index].reduced_chi_squared = avg_reduced_chi_squared;
  state.source_states[index].chi_squared_per_meta.emplace_back(avg_reduced_chi_squared);
  state.source_states[index].duration += duration;
  state.source_states[index].iterations += iterations;
  state.source_states[index].iterations_per_meta.emplace_back(iterations);
  state.source_states[index].stop_reason = stop_reason;
  state.source_states[index].flags = flags;
}

void FlexibleModelFittingIterativeTask::fitSource(SourceGroupInterface& group, SourceInterface& source, int index, FittingState& state) const {

  //////////////////////////////////////////////
  // Determine size of fitted area and if needed downsize factor

  double fit_size = 0;
  for (auto frame : m_frames) {
    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(source, frame_index)) {
      auto psf_property = source.getProperty<SourcePsfProperty>(frame_index);
      auto stamp_rect = getFittingRect(source, frame_index);
      fit_size = std::max(fit_size, stamp_rect.getWidth() * stamp_rect.getHeight() /
          (psf_property.getPixelSampling() * psf_property.getPixelSampling()));
    }
  }

  double down_scaling = m_scale_factor;
  if (fit_size > m_max_fit_size * 2.0) {
    down_scaling *= sqrt(double(m_max_fit_size) / fit_size);
    logger.warn() << "Exceeding max fit size: " << fit_size << " / " << m_max_fit_size
        << " scaling factor: " << down_scaling;
  }

  //////////////////////////////////////////////
  // Prepare parameters

  FlexibleModelFittingParameterManager parameter_manager;
  ModelFitting::EngineParameterManager engine_parameter_manager{};
  int n_free_parameters = fitSourcePrepareParameters(
      parameter_manager, engine_parameter_manager, source, index, state);

  ///////////////////////////////////////////////////////////////////////////////////
  // Add models for all frames
  ResidualEstimator res_estimator {};
  int n_good_pixels = 0;
  int valid_frames = fitSourcePrepareModels(
      parameter_manager, res_estimator, n_good_pixels, group, source, index, state, down_scaling);

  ///////////////////////////////////////////////////////////////////////////////
  // Check that we had enough data for the fit

  Flags flags = Flags::NONE;

  if (valid_frames == 0) {
    flags = Flags::OUTSIDE;
  }
  else if (n_good_pixels < n_free_parameters) {
    flags = Flags::INSUFFICIENT_DATA;
  }

  // Do not run the model fitting for the flags above
  if (flags != Flags::NONE) {
    return;
  }

  if (down_scaling < 1.0) {
    flags |= Flags::DOWNSAMPLED;
  }


  ////////////////////////////////////////////////////////////////////////////////
  // Add priors
  for (auto prior : m_priors) {
    prior->setupPrior(parameter_manager, source, res_estimator);
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Model fitting

  auto engine = LeastSquareEngineManager::create(m_least_squares_engine, m_max_iterations);
  auto solution = engine->solveProblem(engine_parameter_manager, res_estimator);

  auto iterations = solution.iteration_no;
  auto stop_reason = solution.engine_stop_reason;
  if (solution.status_flag == LeastSquareSummary::ERROR) {
    flags |= Flags::ERROR;
  }
  auto duration = solution.duration;

  ////////////////////////////////////////////////////////////////////////////////////
  // compute chi squared

  SeFloat avg_reduced_chi_squared =  fitSourceComputeChiSquared(parameter_manager, group, source, index, state);

  ////////////////////////////////////////////////////////////////////////////////////
  // update state with results
  fitSourceUpdateState(parameter_manager, source, avg_reduced_chi_squared, duration, iterations, stop_reason, flags, solution,
                       index, state);
}

void FlexibleModelFittingIterativeTask::updateCheckImages(SourceGroupInterface& group,
  double pixel_scale, FittingState& state) const {

  // recreate parameters

  FlexibleModelFittingParameterManager parameter_manager;
  ModelFitting::EngineParameterManager engine_parameter_manager {};

  int index = 0;
  for (auto& src : group) {
    for (auto parameter : m_parameters) {
      auto free_parameter = std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter);

      if (free_parameter != nullptr) {
        // Initialize with the values from the current iteration run
        parameter_manager.addParameter(src, parameter,
            free_parameter->create(parameter_manager, engine_parameter_manager, src,
                state.source_states[index].parameters_initial_values.at(free_parameter->getId()),
                state.source_states[index].parameters_values.at(free_parameter->getId())));
      } else {
        parameter_manager.addParameter(src, parameter,
            parameter->create(parameter_manager, engine_parameter_manager, src));
      }
    }
    index++;
  }

  for (auto& src : group) {
    for (auto frame : m_frames) {
      int frame_index = frame->getFrameNb();

      if (isFrameValid(src, frame_index)) {
        auto stamp_rect = getFittingRect(src, frame_index);

        auto frame_model = createFrameModel(src, pixel_scale, parameter_manager, frame, stamp_rect);
        auto final_stamp = frame_model.getImage();

        auto weight_image = createWeightImage(src, frame_index);

        {
          auto debug_image = CheckImages::getInstance().getModelFittingImage(frame_index);
          if (debug_image) {
            ImageAccessor<SeFloat> debug_accessor(debug_image);
            for (int x = 0; x < final_stamp->getWidth(); x++) {
              for (int y = 0; y < final_stamp->getHeight(); y++) {
                auto x_coord = stamp_rect.getTopLeft().m_x + x;
                auto y_coord = stamp_rect.getTopLeft().m_y + y;
                debug_image->setValue(x_coord, y_coord,
                                      debug_accessor.getValue(x_coord, y_coord) + final_stamp->getValue(x, y));
              }
            }
          }
        }

        {
          auto window_image = CheckImages::getInstance().getFittingWindowImage(frame_index);
          if (window_image) {
            ImageAccessor<int> window_accessor(window_image);
            for (int x = 0; x < final_stamp->getWidth(); x++) {
              for (int y = 0; y < final_stamp->getHeight(); y++) {
                auto x_coord = stamp_rect.getTopLeft().m_x + x;
                auto y_coord = stamp_rect.getTopLeft().m_y + y;
                if (weight_image->getValue(x, y) > 0.0) {
                  window_image->setValue(x_coord, y_coord, window_accessor.getValue(x_coord, y_coord) + 2);
                } else {
                  window_image->setValue(x_coord, y_coord, window_accessor.getValue(x_coord, y_coord) + 1);
                }
              }
            }
          }
        }

      }
    }
  }
}

SeFloat FlexibleModelFittingIterativeTask::computeChiSquaredForFrame(std::shared_ptr<const Image<SeFloat>> image,
    std::shared_ptr<const Image<SeFloat>> model, std::shared_ptr<const Image<SeFloat>> weights, int& data_points) const {
  double reduced_chi_squared = 0.0;
  data_points = 0;

  ImageAccessor<SeFloat> imageAccessor(image), modelAccessor(model);
  ImageAccessor<SeFloat> weightAccessor(weights);

  for (int y=0; y < image->getHeight(); y++) {
    for (int x=0; x < image->getWidth(); x++) {
      double tmp = imageAccessor.getValue(x, y) - modelAccessor.getValue(x, y);
      reduced_chi_squared += tmp * tmp * weightAccessor.getValue(x, y) * weightAccessor.getValue(x, y);
      if (weightAccessor.getValue(x, y) > 0) {
        data_points++;
      }
    }
  }
  return reduced_chi_squared;
}

SeFloat FlexibleModelFittingIterativeTask::computeChiSquared(SourceGroupInterface& group, SourceInterface& source, int index,
    double pixel_scale, FlexibleModelFittingParameterManager& manager, int& total_data_points, FittingState& state) const {
  SeFloat total_chi_squared = 0;
  total_data_points = 0;
  int valid_frames = 0;
  for (auto frame : m_frames) {
    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(source, frame_index)) {
      valid_frames++;
      auto stamp_rect = getFittingRect(source, frame_index);
      auto frame_model = createFrameModel(source, pixel_scale, manager, frame, stamp_rect);
      auto final_stamp = frame_model.getImage();

      auto image = createImageCopy(source, frame_index);
      auto deblend_image = createDeblendImage(group, source, index, frame, state);
      for (int y = 0; y < image->getHeight(); ++y) {
        for (int x = 0; x < image->getWidth(); ++x) {
          image->at(x, y) -= deblend_image->at(x, y);
        }
      }

      auto weight = createWeightImage(source, frame_index);

      int data_points = 0;
      SeFloat chi_squared = computeChiSquaredForFrame(image, final_stamp, weight, data_points);

      total_data_points += data_points;
      total_chi_squared += chi_squared;
    }
  }

  return total_chi_squared;
}

}

