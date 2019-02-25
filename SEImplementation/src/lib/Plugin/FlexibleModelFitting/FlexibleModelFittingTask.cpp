/*
 * FlexibleModelFittingTask.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#include <mutex>

#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/TransformedModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LevmarEngine.h"
#include "ModelFitting/Engine/LogChiSquareComparator.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"
#include <SEImplementation/Image/ImagePsf.h>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include <SEImplementation/Plugin/Psf/PsfProperty.h>
#include <SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h>
#include <SEImplementation/Plugin/Jacobian/Jacobian.h>

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFitting.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTask.h"

#include "SEImplementation/CheckImages/CheckImages.h"

namespace SExtractor {

using namespace ModelFitting;


namespace {

void printLevmarInfo(std::array<double,10> info) {
  std::cerr << "\nMinimization info:\n";
  std::cerr << "  ||e||_2 at initial p: " << info[0] << '\n';
  std::cerr << "  ||e||_2: " << info[1] << '\n';
  std::cerr << "  ||J^T e||_inf: " << info[2] << '\n';
  std::cerr << "  ||Dp||_2: " << info[3] << '\n';
  std::cerr << "  mu/max[J^T J]_ii: " << info[4] << '\n';
  std::cerr << "  # iterations: " << info[5] << '\n';
  switch ((int)info[6]) {
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

FlexibleModelFittingTask::FlexibleModelFittingTask(unsigned int max_iterations,
    std::vector<std::shared_ptr<FlexibleModelFittingParameter>> parameters,
    std::vector<std::shared_ptr<FlexibleModelFittingFrame>> frames,
    std::vector<std::shared_ptr<FlexibleModelFittingPrior>> priors)
  : m_max_iterations(max_iterations), m_parameters(parameters), m_frames(frames), m_priors(priors) {}

bool FlexibleModelFittingTask::isFrameValid(SourceGroupInterface& group, int frame_index) const {
  auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  return stamp_rect.getWidth() > 0 && stamp_rect.getHeight() > 0;
}

std::tuple<double, double, double, double> FlexibleModelFittingTask::computeJacobianForFrame(SourceGroupInterface& group, int frame_index) const {
  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_coordinates = frame->getCoordinateSystem();
  auto& detection_group_stamp = group.getProperty<DetectionFrameGroupStamp>();
  auto detection_frame_coordinates = group.begin()->getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  double x = detection_group_stamp.getTopLeft().m_x + detection_group_stamp.getStamp().getWidth() / 2.0;
  double y = detection_group_stamp.getTopLeft().m_y + detection_group_stamp.getStamp().getHeight() / 2.0;

  auto frame_origin = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(x, y)));
  auto frame_dx = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(x+1.0, y)));
  auto frame_dy = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(x, y+1.0)));

  return std::make_tuple(frame_dx.m_x - frame_origin.m_x, frame_dx.m_y - frame_origin.m_y,
      frame_dy.m_x - frame_origin.m_x, frame_dy.m_y - frame_origin.m_y);
}

std::shared_ptr<VectorImage<SeFloat>> FlexibleModelFittingTask::createImageCopy(
    SourceGroupInterface& group, int frame_index) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_image = frame->getSubtractedImage();

  auto rect =  group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  auto image = VectorImage<SeFloat>::create(rect.getWidth(), rect.getHeight());
  for (int y=0; y < rect.getHeight(); y++) {
    for (int x=0; x < rect.getWidth(); x++) {
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

  for (int y=0; y < rect.getHeight(); y++) {
    for (int x=0; x < rect.getWidth(); x++) {
      auto back_var = variance_map->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y);
      if (saturation > 0 && frame_image->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y) > saturation) {
        weight->at(x, y) = 0;
      } else if (weight->at(x, y) > 0) {
        if (gain > 0.0) {
          weight->at(x, y) = sqrt(1.0 / (back_var + frame_image->getValue(rect.getTopLeft().m_x + x, rect.getTopLeft().m_y + y) / gain));
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
    double pixel_scale, FlexibleModelFittingParameterManager& manager, std::shared_ptr<FlexibleModelFittingFrame> frame) const {

  int frame_index = frame->getFrameNb();

  auto frame_coordinates =
      group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame()->getCoordinateSystem();
  auto ref_coordinates =
      group.begin()->getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);
  auto group_psf = group.getProperty<PsfProperty>(frame_index).getPsf();
  auto jacobian = group.getProperty<JacobianGroup>(frame_index).asTuple();

  std::vector<PointModel> point_models;
  std::vector<TransformedModel> extended_models;

  for (auto& source : group) {
    for (auto model : frame->getModels()) {
      model->addForSource(manager, source, point_models, extended_models, jacobian, ref_coordinates, frame_coordinates, stamp_rect.getTopLeft());
    }
  }

  // Full frame model with all sources
  FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model(
    pixel_scale, (size_t) stamp_rect.getWidth(), (size_t) stamp_rect.getHeight(),
    {}, std::move(point_models), std::move(extended_models), group_psf);

  return frame_model;
}


void FlexibleModelFittingTask::computeProperties(SourceGroupInterface& group) const {
  double pixel_scale = 1;
  FlexibleModelFittingParameterManager parameter_manager;
  ModelFitting::EngineParameterManager engine_parameter_manager {};

  {
    std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

    // Prepare parameters
    for (auto& source : group) {
      for (auto parameter : m_parameters) {
        parameter_manager.addParameter(source, parameter, parameter->create(parameter_manager, engine_parameter_manager, source));
      }
    }
  }

  // Add models for all frames
  ResidualEstimator res_estimator {};

  int valid_frames = 0;
  for (auto frame : m_frames) {
    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(group, frame_index)) {
      valid_frames++;

      auto frame_model = createFrameModel(group, pixel_scale, parameter_manager, frame);

      auto image = createImageCopy(group, frame_index);
      auto weight = createWeightImage(group, frame_index);

      // Setup residuals
      auto data_vs_model =
          createDataVsModelResiduals(image, std::move(frame_model), weight, LogChiSquareComparator{});
      res_estimator.registerBlockProvider(std::move(data_vs_model));
    }
  }

  if (valid_frames == 0) {
    // Can't do model fitting as no measurement frame overlaps the detected source
    // We still need to provide a property
    for (auto& source : group) {
      std::unordered_map<int, double> dummy_values;
      for (auto parameter : m_parameters) {
          dummy_values[parameter->getId()] = 99;
      }
      source.setProperty<FlexibleModelFitting>(0, 99, dummy_values, dummy_values);
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
  LevmarEngine engine {m_max_iterations, 1E-6, 1E-6, 1E-6, 1E-6, 1E-4};
  auto solution = engine.solveProblem(engine_parameter_manager, res_estimator);
  printLevmarInfo(boost::any_cast<std::array<double,10>>(solution.underlying_framework_info));
  size_t iterations = (size_t) boost::any_cast<std::array<double,10>>(solution.underlying_framework_info)[5];
  SeFloat avg_reduced_chi_squared = computeReducedChiSquared(group, pixel_scale, parameter_manager);

  // Collect parameters for output
  int parameter_index=0;
  for (auto& source : group) {
    std::unordered_map<int, double> parameter_values, parameter_sigmas;
    for (auto parameter : m_parameters) {
      parameter_values[parameter->getId()] = parameter_manager.getParameter(source, parameter)->getValue();
      if (std::dynamic_pointer_cast<FlexibleModelFittingFreeParameter>(parameter)) {
        parameter_sigmas[parameter->getId()] = solution.parameter_sigmas[parameter_index++];
      } else {
        parameter_sigmas[parameter->getId()] = 99.; // FIXME need user defined error margin for dependent parameters
      }
    }
    source.setProperty<FlexibleModelFitting>(iterations, avg_reduced_chi_squared, parameter_values, parameter_sigmas);
  }

  updateCheckImages(group, pixel_scale, parameter_manager);
}

void FlexibleModelFittingTask::updateCheckImages(SourceGroupInterface& group,
    double pixel_scale, FlexibleModelFittingParameterManager& manager) const {

  int frame_id = 0;
  for (auto frame : m_frames) {
    std::vector<PointModel> point_models;
    std::vector<TransformedModel> extended_models;

    int frame_index = frame->getFrameNb();
    // Validate that each frame covers the model fitting region
    if (isFrameValid(group, frame_index)) {
      auto frame_model = createFrameModel(group, pixel_scale, manager, frame);
      auto final_stamp = frame_model.getImage();

      auto stamp_rect = group.getProperty<MeasurementFrameGroupRectangle>(frame_index);

      CheckImages::getInstance().lock();
      auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();

      std::stringstream checkimage_id;
      checkimage_id << m_checkimage_prefix << "_debug_" << frame_id;
      auto debug_image = CheckImages::getInstance().getWriteableCheckImage(checkimage_id.str(),
          frame->getSubtractedImage()->getWidth(),
          frame->getSubtractedImage()->getHeight());

      for (int x=0; x<final_stamp->getWidth(); x++) {
        for (int y=0; y<final_stamp->getHeight(); y++) {
          auto x_coord = stamp_rect.getTopLeft().m_x + x;
          auto y_coord = stamp_rect.getTopLeft().m_y + y;
          debug_image->setValue(x_coord, y_coord, debug_image->getValue(x_coord, y_coord) + final_stamp->getValue(x,y));
        }
      }

      auto residual_image = SubtractImage<SeFloat>::create(frame->getSubtractedImage(), debug_image);

      std::stringstream checkimage_residual_id;
      checkimage_residual_id << m_checkimage_prefix << "_residual_" << frame_id;
      CheckImages::getInstance().setCustomCheckImage(checkimage_residual_id.str(), residual_image);

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
    std::vector<PointModel> point_models;
    std::vector<TransformedModel> extended_models;

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
