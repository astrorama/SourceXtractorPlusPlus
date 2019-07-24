/*
 * FlexibleModelFittingTask.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#include <mutex>
#include <SEImplementation/Image/ImagePsf.h>

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/TransformedModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LevmarEngine.h"

#include "ModelFitting/Engine/ChiSquareComparator.h"
#include "ModelFitting/Engine/LogChiSquareComparator.h"
#include "ModelFitting/Engine/AsinhChiSquareComparator.h"

#include "ModelFitting/Engine/DataVsModelResiduals.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"
#include "SEImplementation/Image/ImagePsf.h"

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFitting.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTask.h"

#include "SEImplementation/CheckImages/CheckImages.h"

namespace SExtractor {

using namespace ModelFitting;


namespace {

__attribute__((unused))
void printLevmarInfo(std::array<double, 10> info) {
  std::cerr << "\nMinimization info:\n";
  std::cerr << "  ||e||_2 at initial p: " << info[0] << '\n';
  std::cerr << "  ||e||_2: " << info[1] << '\n';
  std::cerr << "  ||J^T e||_inf: " << info[2] << '\n';
  std::cerr << "  ||Dp||_2: " << info[3] << '\n';
  std::cerr << "  mu/max[J^T J]_ii: " << info[4] << '\n';
  std::cerr << "  # iterations: " << info[5] << '\n';
  switch ((int) info[6]) {
    case 1:
      std::cerr << "  stopped by small gradient J^T e\n";
      break;
    case 2:
      std::cerr << "  stopped by small Dp\n";
      break;
    case 3:
      std::cerr << "  stopped by itmax\n";
      break;
    case 4:
      std::cerr << "  singular matrix. Restart from current p with increased mu\n";
      break;
    case 5:
      std::cerr << "  no further error reduction is possible. Restart with increased mu\n";
      break;
    case 6:
      std::cerr << "  stopped by small ||e||_2\n";
      break;
    case 7:
      std::cerr << "  stopped by invalid (i.e. NaN or Inf) func values; a user error\n";
      break;
  }
  std::cerr << "  # function evaluations: " << info[7] << '\n';
  std::cerr << "  # Jacobian evaluations: " << info[8] << '\n';
  std::cerr << "  # linear systems solved: " << info[9] << "\n\n";
}

}

FlexibleModelFittingTask::FlexibleModelFittingTask(unsigned int max_iterations, double modified_chi_squared_scale,
                                                   std::vector<std::shared_ptr<FlexibleModelFittingParameter>> parameters,
                                                   std::vector<std::shared_ptr<FlexibleModelFittingFrame>> frames,
                                                   std::vector<std::shared_ptr<FlexibleModelFittingPrior>> priors)
  : m_max_iterations(max_iterations), m_modified_chi_squared_scale(modified_chi_squared_scale),
    m_parameters(parameters), m_frames(frames), m_priors(priors) {}

bool FlexibleModelFittingTask::isFrameValid(SourceGroupInterface& group, int frame_index) const {
  auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  return stamp_rect.getWidth() > 0 && stamp_rect.getHeight() > 0;
}

std::shared_ptr<VectorImage<SeFloat>> FlexibleModelFittingTask::createImageCopy(
  SourceGroupInterface& group, int frame_index) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_image = frame->getSubtractedImage();

  auto rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  auto image = VectorImage<SeFloat>::create(rect.getWidth(), rect.getHeight());
  for (int y = 0; y < rect.getHeight(); y++) {
    for (int x = 0; x < rect.getWidth(); x++) {
      image->at(x, y) = frame_image->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y);
    }
  }

  return image;
}

std::shared_ptr<VectorImage<SeFloat>> FlexibleModelFittingTask::createWeightImage(
  SourceGroupInterface& group, int frame_index) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_image = frame->getSubtractedImage();
  auto frame_image_thresholded = frame->getThresholdedImage();
  auto variance_map = frame->getVarianceMap();

  auto rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  auto weight = VectorImage<SeFloat>::create(rect.getWidth(), rect.getHeight());
  std::fill(weight->getData().begin(), weight->getData().end(), 1);

  auto measurement_frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  SeFloat gain = measurement_frame->getGain();
  SeFloat saturation = measurement_frame->getSaturation();

  for (int y = 0; y < rect.getHeight(); y++) {
    for (int x = 0; x < rect.getWidth(); x++) {
      auto back_var = variance_map->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y);
      if (saturation > 0 && frame_image->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y) > saturation) {
        weight->at(x, y) = 0;
      } else if (weight->at(x, y) > 0) {
        if (gain > 0.0) {
          weight->at(x, y) = sqrt(
            1.0 / (back_var + frame_image->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y) / gain));
        } else {
          weight->at(x, y) = sqrt(1.0 / back_var); // infinite gain
        }
      }
    }
  }

  return weight;
}

FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> FlexibleModelFittingTask::createFrameModel(
  SourceGroupInterface& group,
  double pixel_scale, FlexibleModelFittingParameterManager& manager,
  std::shared_ptr<FlexibleModelFittingFrame> frame) const {

  int frame_index = frame->getFrameNb();

  auto frame_coordinates =
    group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame()->getCoordinateSystem();
  auto ref_coordinates =
    group.begin()->getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  auto psf_property = group.getProperty<PsfProperty>(frame_index);
  auto jacobian = group.getProperty<JacobianGroup>(frame_index).asTuple();

  // The model fitting module expects to get a PSF with a pixel scale, but we have the pixel sampling step size
  // It will be used to compute the rastering grid size, and after convolving with the PSF the result will be
  // downscaled before copied into the frame image.
  // We can multiply here then, as the unit is pixel/pixel, rather than "/pixel or similar
  auto group_psf = ImagePsf(pixel_scale * psf_property.getPixelSampling(), psf_property.getPsf());

  std::vector<ConstantModel> constant_models;
  std::vector<PointModel> point_models;
  std::vector<TransformedModel> extended_models;

  for (auto& source : group) {
    for (auto model : frame->getModels()) {
      model->addForSource(manager, source, constant_models, point_models, extended_models, jacobian, ref_coordinates, frame_coordinates,
                          stamp_rect.getTopLeft());
    }
  }

  // Full frame model with all sources
  FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model(
    pixel_scale, (size_t) stamp_rect.getWidth(), (size_t) stamp_rect.getHeight(),
    std::move(constant_models), std::move(point_models), std::move(extended_models), group_psf);

  return frame_model;
}


void FlexibleModelFittingTask::computeProperties(SourceGroupInterface& group) const {
  double pixel_scale = 1;
  FlexibleModelFittingParameterManager parameter_manager;
  ModelFitting::EngineParameterManager engine_parameter_manager{};
  int n_free_parameters = 0;

  {
    std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

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
  }

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
    // Can't do model fitting as no measurement frame overlaps the detected source, or there are not enough pixels
    // We still need to provide a property
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
      source.setProperty<FlexibleModelFitting>(0, std::numeric_limits<double>::quiet_NaN(), group_flags,
                                               dummy_values, dummy_values);
    }
    return;
  }

  // Add priors
  for (auto& source : group) {
    for (auto prior : m_priors) {
      prior->setupPrior(parameter_manager, source, res_estimator);
    }
  }

  // Model fitting
  LevmarEngine engine{m_max_iterations, 1E-6, 1E-6, 1E-6, 1E-6, 1E-4};
  auto solution = engine.solveProblem(engine_parameter_manager, res_estimator);
  size_t iterations = (size_t) boost::any_cast<std::array<double, 10>>(solution.underlying_framework_info)[5];
  SeFloat avg_reduced_chi_squared = computeReducedChiSquared(group, pixel_scale, parameter_manager);

  // Collect parameters for output
  int parameter_index = 0;
  for (auto& source : group) {
    std::unordered_map<int, double> parameter_values, parameter_sigmas;
    auto source_flags = Flags::NONE;

    for (auto parameter : m_parameters) {
      bool is_dependent_parameter = std::dynamic_pointer_cast<FlexibleModelFittingDependentParameter>(parameter).get();
      bool accesed_by_modelfitting = parameter_manager.isParamAccessed(source, parameter);
      auto modelfitting_parameter = parameter_manager.getParameter(source, parameter);

      if (is_dependent_parameter || accesed_by_modelfitting) {
        parameter_values[parameter->getId()] = modelfitting_parameter->getValue();
        if (!is_dependent_parameter) {
          parameter_sigmas[parameter->getId()] = solution.parameter_sigmas[parameter_index++];
        } else {
          parameter_sigmas[parameter->getId()] = std::numeric_limits<double>::quiet_NaN();
        }
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
    source.setProperty<FlexibleModelFitting>(iterations, avg_reduced_chi_squared, source_flags, parameter_values,
                                             parameter_sigmas);
  }

  updateCheckImages(group, pixel_scale, parameter_manager);
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
      auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();

      auto debug_image = CheckImages::getInstance().getModelFittingImage(frame);
      if (debug_image) {
        CheckImages::getInstance().lock();
        for (int x = 0; x < final_stamp->getWidth(); x++) {
          for (int y = 0; y < final_stamp->getHeight(); y++) {
            auto x_coord = stamp_rect.getTopLeft().m_x + x;
            auto y_coord = stamp_rect.getTopLeft().m_y + y;
            debug_image->setValue(x_coord, y_coord,
                                  debug_image->getValue(x_coord, y_coord) + final_stamp->getValue(x, y));
          }
        }
      }

      CheckImages::getInstance().unlock();
    }
    frame_id++;
  }
}

SeFloat FlexibleModelFittingTask::computeReducedChiSquaredForFrame(std::shared_ptr<const Image<SeFloat>> image,
    std::shared_ptr<const Image<SeFloat>> model, std::shared_ptr<const Image<SeFloat>> weights,
    int nb_of_free_params) const {
  double reduced_chi_squared = 0.0;
  int data_points = 0;
  for (int y=0; y < image->getHeight(); y++) {
    for (int x=0; x < image->getWidth(); x++) {
      double tmp = image->getValue(x, y) - model->getValue(x, y);
      reduced_chi_squared += tmp * tmp * weights->getValue(x, y) * weights->getValue(x, y);
      if (weights->getValue(x, y) > 0) {
        data_points++;
      }
    }
  }
  return reduced_chi_squared / (data_points - nb_of_free_params);
}

SeFloat FlexibleModelFittingTask::computeReducedChiSquared(SourceGroupInterface& group,
    double pixel_scale, FlexibleModelFittingParameterManager& manager) const {

  SeFloat avg_reduced_chi_squared = 0;
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

      SeFloat reduced_chi_squared = computeReducedChiSquaredForFrame(
          image, final_stamp, weight, manager.getParameterNb());
      avg_reduced_chi_squared += reduced_chi_squared;
    }
  }

  return avg_reduced_chi_squared / valid_frames;
}

FlexibleModelFittingTask::~FlexibleModelFittingTask() {
}

}
