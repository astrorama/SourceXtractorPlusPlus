/*
 * MultiframeModelFittingTask.cpp
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#include <mutex>

#include "ElementsKernel/Logging.h"

#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LevmarEngine.h"
#include "ModelFitting/Engine/LogChiSquareComparator.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"

#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/Image/FitsWriter.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"

#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"

#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFitting.h"
#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeSourceModel.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFittingTask.h"

namespace SExtractor {

using namespace ModelFitting;

static Elements::Logging logger = Elements::Logging::getLogger("MultiframeModelFitting");

namespace {

std::mutex debug_image_mutex;


void printLevmarInfo(std::array<double,10> info) {
  static double total_error = 0;
  total_error += info[1];

  std::cout << "\nMinimization info:\n";
  std::cout << "  ||e||_2 at initial p: " << info[0] << '\n';
  std::cout << "  ||e||_2: " << info[1] << '\n';
  std::cout << "  ||J^T e||_inf: " << info[2] << '\n';
  std::cout << "  ||Dp||_2: " << info[3] << '\n';
  std::cout << "  mu/max[J^T J]_ii: " << info[4] << '\n';
  std::cout << "  # iterations: " << info[5] << '\n';
  switch ((int)info[6]) {
  case 1:
    std::cout << "  stopped by small gradient J^T e\n";
    break;
  case 2:
    std::cout << "  stopped by small Dp\n";
    break;
  case 3:
    std::cout << "  stopped by itmax\n";
    break;
  case 4:
    std::cout << "  singular matrix. Restart from current p with increased mu\n";
    break;
  case 5:
    std::cout << "  no further error reduction is possible. Restart with increased mu\n";
    break;
  case 6:
    std::cout << "  stopped by small ||e||_2\n";
    break;
  case 7:
    std::cout << "  stopped by invalid (i.e. NaN or Inf) func values; a user error\n";
    break;
  }
  std::cout << "  # function evaluations: " << info[7] << '\n';
  std::cout << "  # Jacobian evaluations: " << info[8] << '\n';
  std::cout << "  # linear systems solved: " << info[9] << "\n\n";
  std::cout << "  # total error running count: " << total_error << "\n\n";
}

SeFloat computeReducedChiSquared(
  std::shared_ptr<const Image<SeFloat>> image, std::shared_ptr<const Image<SeFloat>> model, std::shared_ptr<const Image<SeFloat>> weights, int nb_of_free_params) {
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

void printDebugChi2(SeFloat reduced_chi_squared) {
  static double total = 0.0;
  static int count = 0;
  static std::vector<SeFloat> chi_squares;

  chi_squares.push_back(reduced_chi_squared);
  total += reduced_chi_squared;
  count++;

  std::sort(chi_squares.begin(), chi_squares.end());

  std::cout << "    Reduced Chi^2: " << reduced_chi_squared << "\n";
  std::cout << "Avg Reduced Chi^2: " << (total / count) << "\n";
//  std::cout << "Med Reduced Chi^2: " << chi_squares[chi_squares.size() / 2] << "\n";
//  std::cout << "90% Reduced Chi^2: " << chi_squares[chi_squares.size() * 9 / 10] << "\n";
}


}

MultiframeModelFittingTask::MultiframeModelFittingTask(unsigned int max_iterations,
    std::vector<std::vector<int>> frame_indices_per_band, std::vector<std::shared_ptr<ImagePsf>> psfs)
  : m_max_iterations(max_iterations),
    m_frame_indices_per_band(frame_indices_per_band),
    m_psfs(psfs)
{
}

std::shared_ptr<VectorImage<SeFloat>> MultiframeModelFittingTask::createImageCopy(
    SourceGroupInterface& group, int frame_index) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_image = frame->getSubtractedImage();

  auto rect = getStampRectangle(group, frame_index);
  auto image = VectorImage<SeFloat>::create(rect.getWidth(), rect.getHeight());
  for (int y=0; y < rect.getHeight(); y++) {
    for (int x=0; x < rect.getWidth(); x++) {
      image->at(x, y) = frame_image->getValue(rect.m_min_coord.m_x + x, rect.m_min_coord.m_y + y);
    }
  }

  return image;
}

std::shared_ptr<VectorImage<SeFloat>> MultiframeModelFittingTask::createWeightImage(
    SourceGroupInterface& group, int frame_index) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_image = frame->getSubtractedImage();
  auto frame_image_thresholded = frame->getThresholdedImage();
  auto variance_map = frame->getVarianceMap();

  auto rect = getStampRectangle(group, frame_index);
  auto weight = VectorImage<SeFloat>::create(rect.getWidth(), rect.getHeight());
  std::fill(weight->getData().begin(), weight->getData().end(), 1);
//  for (int y=0; y < height; y++) {
//    for (int x=0; x < width; x++) {
//      for (auto& source : group) {
//        auto& frame_centroid = source.getProperty<MeasurementFramePixelCentroid>(frame_index);
//        auto frame_x = frame_centroid.getCentroidX();
//        auto frame_y = frame_centroid.getCentroidY();
//
//        auto dx = x + offset.m_x - frame_x;
//        auto dy = y + offset.m_y - frame_y;
//
//        auto& shape_parameters = source.getProperty<ShapeParameters>();
//        auto radius = 1.5 * shape_parameters.getEllipseA();
//
//        if (frame_image_thresholded->getValue(offset.m_x + x, offset.m_y + y) <= 0 || (dx * dx + dy * dy) < radius * radius) {
//          weight->at(x, y) = 1;
//        }
//      }
//    }
//  }

  auto measurement_frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  SeFloat gain = measurement_frame->getGain();
  SeFloat saturation = measurement_frame->getSaturation();

  for (int y=0; y < rect.getHeight(); y++) {
    for (int x=0; x < rect.getWidth(); x++) {
      auto back_var = variance_map->getValue(rect.m_min_coord.m_x + x, rect.m_min_coord.m_y + y);
      if (saturation > 0 && frame_image->getValue(rect.m_min_coord.m_x + x, rect.m_min_coord.m_y + y) > saturation) {
        weight->at(x, y) = 0;
      } else if (weight->at(x, y) > 0) {
        if (gain > 0.0) {
          weight->at(x, y) = sqrt(1.0 / (back_var + frame_image->getValue(rect.m_min_coord.m_x + x, rect.m_min_coord.m_y + y) / gain));
        } else {
          weight->at(x, y) = sqrt(1.0 / back_var); // infinite gain
        }
      }
    }
  }

  return weight;
}

bool MultiframeModelFittingTask::isFrameValid(SourceGroupInterface& group, int frame_index) const {
  auto stamp_rect = getStampRectangle(group, frame_index);
  return stamp_rect.getWidth() > 0 && stamp_rect.getHeight() > 0;
}

MultiframeModelFittingTask::StampRectangle MultiframeModelFittingTask::getStampRectangle(SourceGroupInterface& group, int frame_index) const {
  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_coordinates = frame->getCoordinateSystem();
  auto& detection_group_stamp = group.getProperty<DetectionFrameGroupStamp>();
  auto detection_frame_coordinates = group.begin()->getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  // Get the coordinates of the detection frame group stamp
  auto stamp_top_left = detection_group_stamp.getTopLeft();
  auto width = detection_group_stamp.getStamp().getWidth();
  auto height = detection_group_stamp.getStamp().getHeight();

  // Transform the 4 corner coordinates from detection image
  auto coord1 = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
      stamp_top_left.m_x, stamp_top_left.m_y)));
  auto coord2 = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
      stamp_top_left.m_x + width, stamp_top_left.m_y)));
  auto coord3 = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
      stamp_top_left.m_x + width, stamp_top_left.m_y + height)));
  auto coord4 = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
      stamp_top_left.m_x, stamp_top_left.m_y + height)));

  // Determine the min/max coordinates
  auto min_x = std::min(coord1.m_x, std::min(coord2.m_x, std::min(coord3.m_x, coord4.m_x)));
  auto min_y = std::min(coord1.m_y, std::min(coord2.m_y, std::min(coord3.m_y, coord4.m_y)));
  auto max_x = std::max(coord1.m_x, std::max(coord2.m_x, std::max(coord3.m_x, coord4.m_x)));
  auto max_y = std::max(coord1.m_y, std::max(coord2.m_y, std::max(coord3.m_y, coord4.m_y)));

  PixelCoordinate min_coord, max_coord;
  min_coord.m_x = int(min_x);
  min_coord.m_y = int(min_y);
  max_coord.m_x = int(max_x) + 1;
  max_coord.m_y = int(max_y) + 1;

  // Clip the coordinates to fit the available image
  auto frame_image = frame->getSubtractedImage();
  min_coord.m_x = std::max(0, min_coord.m_x);
  min_coord.m_y = std::max(0, min_coord.m_y);
  max_coord.m_x = std::min(frame_image->getWidth(), max_coord.m_x);
  max_coord.m_y = std::min(frame_image->getHeight(), max_coord.m_y);

  return StampRectangle(min_coord, max_coord);
}

void MultiframeModelFittingTask::computeJacobianForFrame(SourceGroupInterface& group, int frame_index, double jacobian[]) const {
  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_coordinates = frame->getCoordinateSystem();
  auto& detection_group_stamp = group.getProperty<DetectionFrameGroupStamp>();
  auto detection_frame_coordinates = group.begin()->getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  double x = detection_group_stamp.getTopLeft().m_x + detection_group_stamp.getStamp().getWidth() / 2.0;
  double y = detection_group_stamp.getTopLeft().m_y + detection_group_stamp.getStamp().getHeight() / 2.0;

  auto frame_origin = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(x, y)));
  auto frame_dx = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(x+1.0, y)));
  auto frame_dy = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(x, y+1.0)));

  jacobian[0] = frame_dx.m_x - frame_origin.m_x;
  jacobian[1] = frame_dx.m_y - frame_origin.m_y;
  jacobian[2] = frame_dy.m_x - frame_origin.m_x;
  jacobian[3] = frame_dy.m_y - frame_origin.m_y;
}

void MultiframeModelFittingTask::computeProperties(SourceGroupInterface& group) const {
  std::cout << "MultiframeModelFittingTask::computeProperties()\n";

  // Prepare debug images
  if (m_debug_images.size() == 0) {
    for (auto& frame_indices : m_frame_indices_per_band) {
      for (auto frame_index : frame_indices) {
        auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();

        auto debug_image = VectorImage<SeFloat>::create(frame->getOriginalImage()->getWidth(), frame->getOriginalImage()->getHeight());
        const_cast<MultiframeModelFittingTask*>(this)->m_debug_images[frame_index] = debug_image;
      }
    }
  }

  double pixel_scale = 1;
  EngineParameterManager manager {};
  std::vector<std::unique_ptr<MultiframeSourceModel>> source_models;

  // Validate that each frame covers the model fitting region
  std::vector<std::vector<int>> validated_frame_indices_per_band;
  int total_nb_of_valid_frames = 0;
  for (auto& frame_indices : m_frame_indices_per_band) {
    validated_frame_indices_per_band.emplace_back();
    for (auto frame_index : frame_indices) {
      if (isFrameValid(group, frame_index)) {
        total_nb_of_valid_frames++;
        validated_frame_indices_per_band.back().emplace_back(frame_index);
      }
    }
  }

  if (total_nb_of_valid_frames == 0) {
    // Can't do model fitting as no measurement frame overlaps the detected source
    // We still need to provide a property
    for (auto& source : group) {
      source.setProperty<MultiframeModelFitting>(
          nan(""), nan(""),
          nan(""), nan(""),
          nan(""), nan(""),
          std::vector<double>(m_frame_indices_per_band.size(), nan("")),
          std::vector<double>(m_frame_indices_per_band.size(), nan("")),
          std::vector<double>(m_frame_indices_per_band.size(), nan("")),
          0, nan("")
          );
    }

    return;
  }

  // Setup models for all the sources
  for (auto& source : group) {
    source_models.emplace_back(std::unique_ptr<MultiframeSourceModel>(new MultiframeSourceModel(source)));
  }

  ResidualEstimator res_estimator {};
  std::vector<std::shared_ptr<Image<SeFloat>>> images;
  std::vector<std::shared_ptr<Image<SeFloat>>> weights;

  for (unsigned int band_nb=0; band_nb < validated_frame_indices_per_band.size(); band_nb++) {
    auto& frame_indices = validated_frame_indices_per_band[band_nb];
    if (frame_indices.size() == 0) {
      for (auto& source_model : source_models) {
        source_model->createPlaceholderForInactiveBand();
      }
    } else {
      for (auto& source_model : source_models) {
        source_model->createParamsForBand();
      }
      for (auto frame_index : frame_indices) {
        auto stamp_rect = getStampRectangle(group, frame_index);
        auto image = createImageCopy(group, frame_index);
        auto weight = createWeightImage(group, frame_index);

        double jacobian[4];
        computeJacobianForFrame(group, frame_index, jacobian);
        std::cout << "J:\n";
        std::cout << jacobian[0] << " " << jacobian[1] << "\n";
        std::cout << jacobian[2] << " " << jacobian[3] << "\n";

        // Setup source models
        auto frame_coordinates =
            group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame()->getCoordinateSystem();

        std::vector<ExtendedModel> extended_models;
        for (auto& source_model : source_models) {
          source_model->createParamsForFrame(band_nb, frame_index, frame_coordinates, stamp_rect.m_min_coord);
          source_model->addModelsForFrame(frame_index, extended_models);
        }

        // Full frame model with all sources
        FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model(
          pixel_scale, (size_t) stamp_rect.getWidth(), (size_t) stamp_rect.getHeight(),
          {}, {}, std::move(extended_models), *m_psfs[frame_index]);

        // Setup residuals
        auto data_vs_model =
            createDataVsModelResiduals(image, std::move(frame_model), weight, LogChiSquareComparator{});
        res_estimator.registerBlockProvider(std::move(data_vs_model));

        images.emplace_back(image);
        weights.emplace_back(weight);
      }
    }
  }

  // Register all parameters
  for (auto& source_model : source_models) {
    std::cout << "register parameters\n";
    source_model->registerParameters(manager);
  }

  //
  // Perform the minimization
  //

  LevmarEngine engine {m_max_iterations, 1E-6, 1E-6, 1E-6, 1E-6, 1E-4};

  for (auto& source_model : source_models) {
    std::cout << "Before: ";
    source_model->debugPrint();
  }

  auto solution = engine.solveProblem(manager, res_estimator);

  for (auto& source_model : source_models) {
    std::cout << "After:  ";
    source_model->debugPrint();
  }

  printLevmarInfo(boost::any_cast<std::array<double,10>>(solution.underlying_framework_info));
  size_t iterations = (size_t) boost::any_cast<std::array<double,10>>(solution.underlying_framework_info)[5];

  double avg_reduced_chi_squared = 0;
  int image_nb = 0;
  for (unsigned int band_nb=0; band_nb < validated_frame_indices_per_band.size(); band_nb++) {
    auto& frame_indices = validated_frame_indices_per_band[band_nb];
    for (auto frame_index : frame_indices) {
      auto stamp_rect = getStampRectangle(group, frame_index);
      auto stamp_width = stamp_rect.getWidth();
      auto stamp_height = stamp_rect.getHeight();

      std::vector<ExtendedModel> extended_models {};
      std::vector<PointModel> point_models {};
      std::vector<ConstantModel> constant_models;

      int nb_of_params = 0;
      for (auto& source_model : source_models) {
        source_model->addModelsForFrame(frame_index, extended_models);
        nb_of_params += source_model->getNumberOfParameters();
      }

      FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model_after {
        pixel_scale,
        (size_t) stamp_width, (size_t) stamp_height,
        std::move(constant_models),
        std::move(point_models),
        std::move(extended_models),
        *m_psfs[frame_index]
      };
      auto final_stamp = frame_model_after.getImage();

      for (int x=0; x<final_stamp->getWidth(); x++) {
        for (int y=0; y<final_stamp->getHeight(); y++) {
          debug_image_mutex.lock();
          const_cast<MultiframeModelFittingTask*>(this)->m_debug_images[frame_index]->at(
              stamp_rect.m_min_coord.m_x + x, stamp_rect.m_min_coord.m_y + y) += final_stamp->getValue(x,y);
          debug_image_mutex.unlock();
        }
      }

      auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
      auto residual_image = SubtractImage<SeFloat>::create(frame->getOriginalImage(),
            const_cast<MultiframeModelFittingTask*>(this)->m_debug_images[frame_index]);
      const_cast<MultiframeModelFittingTask*>(this)->m_residual_images[frame_index] = residual_image;

      SeFloat reduced_chi_squared = computeReducedChiSquared(
          images[image_nb], final_stamp, weights[image_nb], nb_of_params);
      avg_reduced_chi_squared += reduced_chi_squared;
      printDebugChi2(reduced_chi_squared);

      image_nb++;
    }
  }

  avg_reduced_chi_squared /= total_nb_of_valid_frames;

  // Set the property for all sources
  auto source_iter = group.begin();
  for (auto& source_model : source_models) {
    auto& source = *source_iter;
    ++source_iter;

    auto ic = source_model->getFittedCoordinate();
    auto wc = source_model->getFittedWorldCoordinate();
    source.setProperty<MultiframeModelFitting>(
        ic.m_x, ic.m_y,
        wc.m_alpha, wc.m_delta,
        source_model->getExpRadius(), source_model->getDevRadius(),
        source_model->getFluxes(), source_model->getExpFluxes(), source_model->getDevFluxes(),
        iterations, avg_reduced_chi_squared
        );
  }

  std::cout << "...\n";
}

MultiframeModelFittingTask::~MultiframeModelFittingTask() {
  std::cout << "Writing output images\n";

  // Output debug images
  {
    for (unsigned int band_nb=0; band_nb < m_frame_indices_per_band.size(); band_nb++) {
      auto& frame_indices = m_frame_indices_per_band[band_nb];
      for (auto frame_index : frame_indices) {
        std::stringstream file_name;
        file_name << "debug_" << band_nb << "_" << frame_index << ".fits";
        FitsWriter::writeFile(*(m_debug_images[frame_index]), file_name.str());

        if (m_residual_images[frame_index] != nullptr) {
          std::stringstream res_file_name;
          res_file_name << "res_" << band_nb << "_" << frame_index << ".fits";
          FitsWriter::writeFile(*m_residual_images[frame_index], res_file_name.str());
        }
      }
    }
  }


}


}
