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

#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/Psf/PsfProperty.h"

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
    double scale_factor)
  : m_least_squares_engine(least_squares_engine),
    m_max_iterations(max_iterations), m_modified_chi_squared_scale(modified_chi_squared_scale),
    m_parameters(parameters), m_frames(frames), m_priors(priors), m_scale_factor(scale_factor) {}

FlexibleModelFittingIterativeTask::~FlexibleModelFittingIterativeTask() {
}


namespace {

bool isFrameValid(SourceInterface& source, int frame_index) {
  auto stamp_rect = source.getProperty<MeasurementFrameRectangle>(frame_index); //FIXME
  return stamp_rect.getWidth() > 0 && stamp_rect.getHeight() > 0;
}

std::shared_ptr<VectorImage<SeFloat>> createImageCopy(SourceInterface& source, int frame_index) {
  const auto& frame_images = source.getProperty<MeasurementFrameImages>(frame_index);
  auto rect = source.getProperty<MeasurementFrameRectangle>(frame_index); //FIXME

  auto image = VectorImage<SeFloat>::create(frame_images.getImageChunk(
      LayerSubtractedImage, rect.getTopLeft().m_x, rect.getTopLeft().m_y, rect.getWidth(), rect.getHeight()));

  return image;
}

std::shared_ptr<VectorImage<SeFloat>> createWeightImage(SourceInterface& source, int frame_index) {
  const auto& frame_images = source.getProperty<MeasurementFrameImages>(frame_index);
  auto frame_image = frame_images.getLockedImage(LayerSubtractedImage);
  auto frame_image_thresholded = frame_images.getLockedImage(LayerThresholdedImage);
  auto variance_map = frame_images.getLockedImage(LayerVarianceMap);

  const auto& frame_info = source.getProperty<MeasurementFrameInfo>(frame_index);
  SeFloat gain = frame_info.getGain();
  SeFloat saturation = frame_info.getSaturation();

  auto rect = source.getProperty<MeasurementFrameRectangle>(frame_index); //FIXME
  auto weight = VectorImage<SeFloat>::create(rect.getWidth(), rect.getHeight());

  for (int y = 0; y < rect.getHeight(); y++) {
    for (int x = 0; x < rect.getWidth(); x++) {
      auto back_var = variance_map->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y);
      auto pixel_val = frame_image->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y);
      if (saturation > 0 && pixel_val > saturation) {
        weight->at(x, y) = 0;
      }
      else if (gain > 0.0 && pixel_val > 0.0) {
        weight->at(x, y) = sqrt(1.0 / (back_var + pixel_val / gain));
      }
      else {
        weight->at(x, y) = sqrt(1.0 / back_var); // infinite gain
      }
    }
  }

  return weight;
}
FrameModel<ImagePsf, std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>> createFrameModel(
    SourceInterface& source, double pixel_scale, FlexibleModelFittingParameterManager& manager,
    std::shared_ptr<FlexibleModelFittingFrame> frame) {

  // FIXME: for now still use the group's PSF property, replace by a PSF for the source

  int frame_index = frame->getFrameNb();

  auto frame_coordinates = source.getProperty<MeasurementFrameCoordinates>(frame_index).getCoordinateSystem();
  auto ref_coordinates = source.getProperty<DetectionFrameCoordinates>().getCoordinateSystem();

  auto stamp_rect = source.getProperty<MeasurementFrameRectangle>(frame_index);
  auto psf_property = source.getProperty<PsfProperty>(frame_index);
  auto jacobian = source.getProperty<JacobianSource>(frame_index).asTuple();

  // The model fitting module expects to get a PSF with a pixel scale, but we have the pixel sampling step size
  // It will be used to compute the rastering grid size, and after convolving with the PSF the result will be
  // downscaled before copied into the frame image.
  // We can multiply here then, as the unit is pixel/pixel, rather than "/pixel or similar
  auto group_psf = ImagePsf(pixel_scale * psf_property.getPixelSampling(), psf_property.getPsf());

  std::vector<ConstantModel> constant_models;
  std::vector<PointModel> point_models;
  std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>> extended_models;

  for (auto model : frame->getModels()) {
    model->addForSource(manager, source, constant_models, point_models, extended_models, jacobian, ref_coordinates,
        frame_coordinates, stamp_rect.getTopLeft());
  }

  // Full frame model with all sources
  FrameModel<ImagePsf, std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>> frame_model(
    pixel_scale, (size_t) stamp_rect.getWidth(), (size_t) stamp_rect.getHeight(),
    std::move(constant_models), std::move(point_models), std::move(extended_models), group_psf);

  return frame_model;
}

}


void FlexibleModelFittingIterativeTask::computeProperties(SourceGroupInterface& group) const {



  // FIXME: for now just output dummy properties
  for (auto& source : group) {
    fitSource(source);
    //setDummyProperty(source, Flags::ERROR);
  }
}


// Used to set a dummy property in case of error that contains no result but just an error flag
void FlexibleModelFittingIterativeTask::setDummyProperty(SourceInterface& source, Flags flags) const {
    std::unordered_map<int, double> dummy_values;
    for (auto parameter : m_parameters) {
      dummy_values[parameter->getId()] = std::numeric_limits<double>::quiet_NaN();
    }
    source.setProperty<FlexibleModelFitting>(0, 0, std::numeric_limits<double>::quiet_NaN(), flags,
                                             dummy_values, dummy_values);
}

void FlexibleModelFittingIterativeTask::fitSource(SourceInterface& source) const {
  double pixel_scale = 1 / m_scale_factor;

  //////////////////////////////////////////////
  // Prepare parameters

  FlexibleModelFittingParameterManager parameter_manager;
  ModelFitting::EngineParameterManager engine_parameter_manager{};
  int n_free_parameters = 0;

  for (auto parameter : m_parameters) {
    if (std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter)) {
      ++n_free_parameters;
    }
    parameter_manager.addParameter(source, parameter,
                                   parameter->create(parameter_manager, engine_parameter_manager, source));
  }

  // Reset access checks, as a dependent parameter could have triggered it
  parameter_manager.clearAccessCheck();


  ///////////////////////////////////////////////////////////////////////////////////
  // Add models for all frames
  ResidualEstimator res_estimator {};

  int valid_frames = 0;
  int n_good_pixels = 0;
  for (auto frame : m_frames) {
    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(source, frame_index)) {
      valid_frames++;

      auto frame_model = createFrameModel(source, pixel_scale, parameter_manager, frame);

      auto image = createImageCopy(source, frame_index);
      auto weight = createWeightImage(source, frame_index);

      // count number of pixels that can be used for fitting
      for (int y = 0; y < weight->getHeight(); ++y) {
        for (int x = 0; x < weight->getWidth(); ++x) {
          n_good_pixels += (weight->at(x, y) != 0.);
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

  ///////////////////////////////////////////////////////////////////////////////
  // Check that we had enough data for the fit
  Flags group_flags = Flags::NONE;
  if (valid_frames == 0) {
    group_flags = Flags::OUTSIDE;
  }
  else if (n_good_pixels < n_free_parameters) {
    group_flags = Flags::INSUFFICIENT_DATA;
  }

  if (group_flags != Flags::NONE) {
    setDummyProperty(source, group_flags);
    return;
  }

  // FIXME re-enable
  ////////////////////////////////////////////////////////////////////////////////
//  // Add priors
//  for (auto& source : group) {
//    for (auto prior : m_priors) {
//      prior->setupPrior(parameter_manager, source, res_estimator);
//    }
//  }

  /////////////////////////////////////////////////////////////////////////////////
  // Model fitting

  auto engine = LeastSquareEngineManager::create(m_least_squares_engine, m_max_iterations);
  auto solution = engine->solveProblem(engine_parameter_manager, res_estimator);

  auto iterations = solution.iteration_no;
  auto stop_reason = solution.engine_stop_reason;
  if (solution.status_flag == LeastSquareSummary::ERROR) {
    group_flags |= Flags::ERROR;
  }


  ////////////////////////////////////////////////////////////////////////////////////
  // compute chi squared

  //FIXME reenable
//  int total_data_points = 0;
//  SeFloat avg_reduced_chi_squared = computeChiSquared(group, pixel_scale, parameter_manager, total_data_points);
//
//  int nb_of_free_parameters = 0;
//  for (auto& source : group) {
//    for (auto parameter : m_parameters) {
//      bool is_free_parameter = std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter).get();
//      bool accessed_by_modelfitting = parameter_manager.isParamAccessed(source, parameter);
//      if (is_free_parameter && accessed_by_modelfitting) {
//        nb_of_free_parameters++;
//      }
//    }
//  }
//  avg_reduced_chi_squared /= (total_data_points - nb_of_free_parameters);
  SeFloat avg_reduced_chi_squared = 1.f;

  ////////////////////////////////////////////////////////////////////////////////////
  // Collect parameters for output
    std::unordered_map<int, double> parameter_values, parameter_sigmas;
  auto source_flags = group_flags;

  for (auto parameter : m_parameters) {
    bool is_dependent_parameter = std::dynamic_pointer_cast<FlexibleModelFittingDependentParameter>(parameter).get();
    bool accessed_by_modelfitting = parameter_manager.isParamAccessed(source, parameter);
    auto modelfitting_parameter = parameter_manager.getParameter(source, parameter);

    if (is_dependent_parameter || accessed_by_modelfitting) {
      parameter_values[parameter->getId()] = modelfitting_parameter->getValue();
      parameter_sigmas[parameter->getId()] = parameter->getSigma(parameter_manager, source, solution.parameter_sigmas);
    }
    else {
      // Need to cascade the NaN to any potential dependent parameter
      auto engine_parameter = std::dynamic_pointer_cast<EngineParameter>(modelfitting_parameter);
      if (engine_parameter) {
        engine_parameter->setEngineValue(std::numeric_limits<double>::quiet_NaN());
      }
      parameter_values[parameter->getId()] = std::numeric_limits<double>::quiet_NaN();
      parameter_sigmas[parameter->getId()] = std::numeric_limits<double>::quiet_NaN();
      source_flags |= Flags::PARTIAL_FIT;
    }
  }

  source.setProperty<FlexibleModelFitting>(iterations, stop_reason,
                                           avg_reduced_chi_squared, source_flags,
                                           parameter_values, parameter_sigmas);

  ////////////////////////////////////////////////////////////////////
  // Check images

  updateCheckImages(source, pixel_scale, parameter_manager);
}

void FlexibleModelFittingIterativeTask::updateCheckImages(SourceInterface& source,
    double pixel_scale, FlexibleModelFittingParameterManager& manager) const {

  int frame_id = 0;
  for (auto frame : m_frames) {
    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(source, frame_index)) {
      auto frame_model = createFrameModel(source, pixel_scale, manager, frame);
      auto final_stamp = frame_model.getImage();

      auto stamp_rect = source.getProperty<MeasurementFrameRectangle>(frame_index); // FIXME

      auto debug_image = CheckImages::getInstance().getModelFittingImage(frame_index);
      if (debug_image) {
        ImageAccessor<SeFloat> debugAccessor(debug_image);
        for (int x = 0; x < final_stamp->getWidth(); x++) {
          for (int y = 0; y < final_stamp->getHeight(); y++) {
            auto x_coord = stamp_rect.getTopLeft().m_x + x;
            auto y_coord = stamp_rect.getTopLeft().m_y + y;
            debug_image->setValue(x_coord, y_coord,
                                  debugAccessor.getValue(x_coord, y_coord) + final_stamp->getValue(x, y));
          }
        }
      }
    }
    frame_id++;
  }
}

}



