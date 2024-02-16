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
 * FlexibleModelFittingTask.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#include <mutex>

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/TransformedModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LeastSquareEngineManager.h"
#include "ModelFitting/Engine/LogChiSquareComparator.h"
#include "ModelFitting/Engine/AsinhChiSquareComparator.h"

#include "ModelFitting/Engine/DataVsModelResiduals.h"

#include "SEFramework/Image/ImageAccessor.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"
#include "SEImplementation/Image/ImagePsf.h"

#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/ReferenceCoordinates/ReferenceCoordinates.h"


#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFitting.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTask.h"

#include "SEImplementation/CheckImages/CheckImages.h"

namespace SourceXtractor {

using namespace ModelFitting;

static auto logger = Elements::Logging::getLogger("FlexibleModelFitting");

FlexibleModelFittingTask::FlexibleModelFittingTask(const std::string &least_squares_engine,
    unsigned int max_iterations, double modified_chi_squared_scale,
    std::vector<std::shared_ptr<FlexibleModelFittingParameter>> parameters,
    std::vector<std::shared_ptr<FlexibleModelFittingFrame>> frames,
    std::vector<std::shared_ptr<FlexibleModelFittingPrior>> priors,
    double scale_factor)
  : m_least_squares_engine(least_squares_engine),
    m_max_iterations(max_iterations), m_modified_chi_squared_scale(modified_chi_squared_scale),
    m_parameters(parameters), m_frames(frames), m_priors(priors), m_scale_factor(scale_factor) {}

bool FlexibleModelFittingTask::isFrameValid(SourceGroupInterface& group, int frame_index) const {
  auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  return stamp_rect.getWidth() > 0 && stamp_rect.getHeight() > 0;
}

std::shared_ptr<VectorImage<SeFloat>> FlexibleModelFittingTask::createImageCopy(
  SourceGroupInterface& group, int frame_index) const {
  const auto& frame_images = group.begin()->getProperty<MeasurementFrameImages>(frame_index);
  auto rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  auto image = VectorImage<SeFloat>::create(frame_images.getImageChunk(
      LayerSubtractedImage, rect.getTopLeft().m_x, rect.getTopLeft().m_y, rect.getWidth(), rect.getHeight()));

  return image;
}

std::shared_ptr<VectorImage<SeFloat>> FlexibleModelFittingTask::createWeightImage(
  SourceGroupInterface& group, int frame_index) const {
  const auto& frame_images = group.begin()->getProperty<MeasurementFrameImages>(frame_index);

  auto frame_image = frame_images.getLockedImage(LayerSubtractedImage);
  auto frame_image_thresholded = frame_images.getLockedImage(LayerThresholdedImage);
  auto variance_map = frame_images.getLockedImage(LayerVarianceMap);

  const auto& frame_info = group.begin()->getProperty<MeasurementFrameInfo>(frame_index);
  SeFloat gain = frame_info.getGain();
  SeFloat saturation = frame_info.getSaturation();

  auto rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
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

FrameModel<ImagePsf, std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>> FlexibleModelFittingTask::createFrameModel(
  SourceGroupInterface& group,
  double pixel_scale, FlexibleModelFittingParameterManager& manager,
  std::shared_ptr<FlexibleModelFittingFrame> frame) const {

  int frame_index = frame->getFrameNb();

  auto frame_coordinates =
    group.begin()->getProperty<MeasurementFrameCoordinates>(frame_index).getCoordinateSystem();
  auto ref_coordinates =
    group.begin()->getProperty<ReferenceCoordinates>().getCoordinateSystem();

  auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  auto psf_property = group.getProperty<PsfProperty>(frame_index);
  auto jacobian = group.getProperty<JacobianGroup>(frame_index).asTuple();

  if (psf_property.getPsf() == nullptr) {
    throw Elements::Exception() << "Missing PSF. No PSF mode is not supported in legacy model fitting";
  }

  // The model fitting module expects to get a PSF with a pixel scale, but we have the pixel sampling step size
  // It will be used to compute the rastering grid size, and after convolving with the PSF the result will be
  // downscaled before copied into the frame image.
  // We can multiply here then, as the unit is pixel/pixel, rather than "/pixel or similar
  auto group_psf = ImagePsf(pixel_scale * psf_property.getPixelSampling(), psf_property.getPsf());

  std::vector<ConstantModel> constant_models;
  std::vector<PointModel> point_models;
  std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>> extended_models;

  double model_size = std::max(stamp_rect.getWidth(), stamp_rect.getHeight());
  for (auto& source : group) {
    for (auto model : frame->getModels()) {
      model->addForSource(manager, source, constant_models, point_models, extended_models, model_size,
          jacobian, ref_coordinates, frame_coordinates, stamp_rect.getTopLeft());
    }
  }

  // Full frame model with all sources
  FrameModel<ImagePsf, std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>> frame_model(
    pixel_scale, (size_t) stamp_rect.getWidth(), (size_t) stamp_rect.getHeight(),
    std::move(constant_models), std::move(point_models), std::move(extended_models), group_psf);

  return frame_model;
}


void FlexibleModelFittingTask::computeProperties(SourceGroupInterface& group) const {
  double pixel_scale = 1 / m_scale_factor;
  FlexibleModelFittingParameterManager parameter_manager;
  ModelFitting::EngineParameterManager engine_parameter_manager{};
  int n_free_parameters = 0;

  // Prepare parameters
  for (auto& source : group) {
    for (auto parameter : m_parameters) {
      if (std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter)) {
        ++n_free_parameters;
      }
      parameter_manager.addParameter(source, parameter,
                                     parameter->create(parameter_manager, engine_parameter_manager, source));
    }
  }

  try {
    // Reset access checks, as a dependent parameter could have triggered it
    parameter_manager.clearAccessCheck();

    // Add models for all frames
    ResidualEstimator res_estimator{};

    int valid_frames = 0;
    int n_good_pixels = 0;
    for (auto frame : m_frames) {
      int frame_index = frame->getFrameNb();
      // Validate that each frame covers the model fitting region
      if (isFrameValid(group, frame_index)) {
        valid_frames++;

        auto frame_model = createFrameModel(group, pixel_scale, parameter_manager, frame);

        auto image = createImageCopy(group, frame_index);
        auto weight = createWeightImage(group, frame_index);

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

    // Check that we had enough data for the fit
    Flags group_flags = Flags::NONE;
    if (valid_frames == 0) {
      group_flags = Flags::OUTSIDE;
    }
    else if (n_good_pixels < n_free_parameters) {
      group_flags = Flags::INSUFFICIENT_DATA;
    }

    if (group_flags != Flags::NONE) {
      setDummyProperty(group, parameter_manager, group_flags);
      return;
    }

    // Add priors
    for (auto& source : group) {
      for (auto prior : m_priors) {
        prior->setupPrior(parameter_manager, source, res_estimator);
      }
    }

    // Model fitting

    // FIXME we can no longer specify different settings with LeastSquareEngineManager!!
    //  LevmarEngine engine{m_max_iterations, 1E-3, 1E-6, 1E-6, 1E-6, 1E-4};
    auto engine = LeastSquareEngineManager::create(m_least_squares_engine, m_max_iterations);
    auto solution = engine->solveProblem(engine_parameter_manager, res_estimator);
    auto iterations = solution.iteration_no;
    auto stop_reason = solution.engine_stop_reason;
    switch (solution.status_flag) {
      case LeastSquareSummary::MEMORY:
        group_flags |= (Flags::MEMORY | Flags::ERROR);
        break;
      case LeastSquareSummary::ERROR:
        group_flags |= Flags::ERROR;
        break;
      default:
        break;
    }

    int total_data_points = 0;
    SeFloat avg_reduced_chi_squared = computeChiSquared(group, pixel_scale, parameter_manager, total_data_points);

    int nb_of_free_parameters = 0;
    for (auto& source : group) {
      for (auto parameter : m_parameters) {
        bool is_free_parameter = std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter).get();
        bool accessed_by_modelfitting = parameter_manager.isParamAccessed(source, parameter);
        if (is_free_parameter && accessed_by_modelfitting) {
          nb_of_free_parameters++;
        }
      }
    }
    avg_reduced_chi_squared /= (total_data_points - nb_of_free_parameters);

    // Collect parameters for output
    for (auto& source : group) {
      std::unordered_map<int, double> parameter_values, parameter_sigmas;
      auto source_flags = group_flags;

      for (auto parameter : m_parameters) {
        bool is_dependent_parameter = std::dynamic_pointer_cast<FlexibleModelFittingDependentParameter>(parameter).get();
        bool is_constant_parameter = std::dynamic_pointer_cast<FlexibleModelFittingConstantParameter>(parameter).get();
        bool accessed_by_modelfitting = parameter_manager.isParamAccessed(source, parameter);
        auto modelfitting_parameter = parameter_manager.getParameter(source, parameter);

        if (is_constant_parameter || is_dependent_parameter || accessed_by_modelfitting) {
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
                                               avg_reduced_chi_squared, solution.duration, source_flags,
                                               parameter_values, parameter_sigmas,
                                               std::vector<SeFloat>({avg_reduced_chi_squared}),
                                               std::vector<int>({(int) iterations}), (int) 1,
                                               std::vector<SeFloat>({99.f}), std::vector<SeFloat>({99.f}));
    }
    updateCheckImages(group, pixel_scale, parameter_manager);

  }
  catch (const Elements::Exception& e) {
    logger.error() << "An exception occured during model fitting:  " << e.what();
    setDummyProperty(group, parameter_manager, Flags::ERROR);
  }
}

// Used to set a dummy property in case of error that contains no result but just an error flag
void FlexibleModelFittingTask::setDummyProperty(SourceGroupInterface& group,
                                                FlexibleModelFittingParameterManager& parameter_manager,
                                                Flags flags) const {
  for (auto& source : group) {
    std::unordered_map<int, double> dummy_values;
    for (auto parameter : m_parameters) {
      auto modelfitting_parameter = parameter_manager.getParameter(source, parameter);
      auto manual_parameter = std::dynamic_pointer_cast<ManualParameter>(modelfitting_parameter);
      if (manual_parameter) {
        manual_parameter->setValue(std::numeric_limits<double>::quiet_NaN());
      }
      dummy_values[parameter->getId()] = std::numeric_limits<double>::quiet_NaN();
    }
    source.setProperty<FlexibleModelFitting>(0, 0, std::numeric_limits<double>::quiet_NaN(), 0., flags,
                                             dummy_values, dummy_values,
                                             std::vector<SeFloat>(1), std::vector<int>(1), 0,
                                             std::vector<SeFloat>({99.f}), std::vector<SeFloat>({99.f}));
  }
}

void FlexibleModelFittingTask::updateCheckImages(SourceGroupInterface& group,
    double pixel_scale, FlexibleModelFittingParameterManager& manager) const {

  int frame_id = 0;
  for (auto frame : m_frames) {
    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(group, frame_index)) {
      auto frame_model = createFrameModel(group, pixel_scale, manager, frame);
      auto final_stamp = frame_model.getImage();

      auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);

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

SeFloat FlexibleModelFittingTask::computeChiSquaredForFrame(std::shared_ptr<const Image<SeFloat>> image,
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

SeFloat FlexibleModelFittingTask::computeChiSquared(SourceGroupInterface& group,
    double pixel_scale, FlexibleModelFittingParameterManager& manager, int& total_data_points) const {

  SeFloat total_chi_squared = 0;
  total_data_points = 0;
  int valid_frames = 0;
  for (auto frame : m_frames) {
    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(group, frame_index)) {
      valid_frames++;
      auto frame_model = createFrameModel(group, pixel_scale, manager, frame);
      auto final_stamp = frame_model.getImage();

      auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
      auto image = createImageCopy(group, frame_index);
      auto weight = createWeightImage(group, frame_index);

      int data_points = 0;
      SeFloat chi_squared = computeChiSquaredForFrame(
          image, final_stamp, weight, data_points);

      total_data_points += data_points;
      total_chi_squared += chi_squared;
    }
  }

  return total_chi_squared;
}

FlexibleModelFittingTask::~FlexibleModelFittingTask() {
}

}
