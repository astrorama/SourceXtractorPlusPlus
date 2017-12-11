/*
 * MultiframeModelFittingTask.cpp
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

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

#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFittingTask.h"

namespace SExtractor {

using namespace ModelFitting;

static Elements::Logging logger = Elements::Logging::getLogger("MultiframeModelFitting");

namespace {

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


// Creates the weight image
std::shared_ptr<VectorImage<SeFloat>> MultiframeModelFittingTask::createWeightImage(
    SourceGroupInterface& group, int width, int height, PixelCoordinate offset, int frame_index) const {

  auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
  auto frame_image = frame->getSubtractedImage();
  auto frame_image_thresholded = frame->getThresholdedImage();

  auto weight = VectorImage<SeFloat>::create(width, height);
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
  auto back_var = measurement_frame->getBackgroundRMS();
  std::cout << "back_var " << back_var << "\n";
  back_var *= back_var; // RMS -> variance
  SeFloat gain = measurement_frame->getGain();
  SeFloat saturation = measurement_frame->getSaturation();

  for (int y=0; y < height; y++) {
    for (int x=0; x < width; x++) {
      if (saturation > 0 && frame_image->getValue(offset.m_x + x, offset.m_y + y) > saturation) {
        weight->at(x, y) = 0;
      } else if (weight->at(x, y) > 0) {
        if (gain > 0.0) {
          weight->at(x, y) = sqrt(1.0 / (back_var + frame_image->getValue(offset.m_x + x, offset.m_y + y) / gain));
        } else {
          weight->at(x, y) = sqrt(1.0 / back_var); // infinite gain
        }
      }
    }
  }

  return weight;
}


void MultiframeModelFittingTask::computeProperties(SourceGroupInterface& group) const {
  //auto& group_stamp = group.getProperty<MeasurementFrameGroupStamp>().getStamp();
  auto& group_stamp = group.getProperty<DetectionFrameGroupStamp>();
  auto detection_frame_coordinates = group.begin()->getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  std::cout << "MultiframeModelFittingTask::computeProperties()\n";

  static std::vector<std::shared_ptr<VectorImage<SeFloat>>> debug_images;

  if (debug_images.size() == 0) {
    for (auto& frame_indices : m_frame_indices_per_band) {
      for (auto frame_index : frame_indices) {
        auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();

        auto debug_image = VectorImage<SeFloat>::create(frame->getOriginalImage()->getWidth(), frame->getOriginalImage()->getHeight());
        debug_images.emplace_back(debug_image);
      }
    }
  }

  double pixel_scale = 1;

  EngineParameterManager manager {};
  std::vector<std::unique_ptr<MultiframeSourceModel>> source_models;

  std::vector<std::shared_ptr<MeasurementImageFrame>> frames;
  std::vector<int> valid_frame_indices;
  std::vector<std::shared_ptr<ImagePsf>> psfs;
  std::vector<bool> first_frames;

  std::vector<PixelCoordinate> min_coords, max_coords;

  int i = 0;
  for (auto& frame_indices : m_frame_indices_per_band) {
    bool first_frame_of_band = true;
    for (auto frame_index : frame_indices) {
      auto frame = group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame();
      auto frame_coordinates = frame->getCoordinateSystem();

      auto stamp_top_left = group_stamp.getTopLeft();
      auto top_left = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(
          stamp_top_left.m_x, stamp_top_left.m_y)));

      PixelCoordinate min_coord(int(top_left.m_x), int(top_left.m_y));
      PixelCoordinate max_coord = min_coord + PixelCoordinate(group_stamp.getStamp().getWidth(), group_stamp.getStamp().getHeight());

      auto frame_image = frame->getSubtractedImage();
      auto frame_image_thresholded = frame->getThresholdedImage();

      // FIXME fixed 5 pixel border for now
      int border = 0;//fixme
      min_coord.m_x = std::max(0, min_coord.m_x - border);
      min_coord.m_y = std::max(0, min_coord.m_y - border);
      max_coord.m_x = std::min(frame_image->getWidth()-1, max_coord.m_x + border);
      max_coord.m_y = std::min(frame_image->getHeight()-1, max_coord.m_y + border);

      int stamp_width = max_coord.m_x - min_coord.m_x;
      int stamp_height = max_coord.m_y - min_coord.m_y;

      if (stamp_width > 0 && stamp_height > 0) {
        valid_frame_indices.emplace_back(frame_index);
        psfs.emplace_back(m_psfs[i++]);
        first_frames.emplace_back(first_frame_of_band);
        first_frame_of_band = false;

        min_coords.emplace_back(min_coord);
        max_coords.emplace_back(max_coord);
        frames.push_back(frame);
      }
    }
  }

  // Setup models for all the sources
  for (auto& source : group) {
    source_models.emplace_back(std::unique_ptr<MultiframeSourceModel>(new MultiframeSourceModel(source)));
  }

  ResidualEstimator res_estimator {};

  std::vector<std::shared_ptr<Image<SeFloat>>> images;
  std::vector<std::shared_ptr<Image<SeFloat>>> weights;

  for (int i = 0; i < (int) valid_frame_indices.size(); i++) {
    auto min_coord = min_coords[i];
    auto max_coord = max_coords[i];
    int stamp_width = max_coord.m_x - min_coord.m_x;
    int stamp_height = max_coord.m_y - min_coord.m_y;

    auto frame_image = frames[i]->getSubtractedImage();
    auto frame_image_thresholded = frames[i]->getThresholdedImage();

    auto image = VectorImage<SeFloat>::create(stamp_width, stamp_height);
    for (int y=0; y<stamp_height; y++) {
      for (int x=0; x<stamp_width; x++) {
        image->at(x, y) = frame_image->getValue(min_coord.m_x + x, min_coord.m_y + y);
      }
    }

    auto weight = createWeightImage(group, stamp_width, stamp_height, min_coord, valid_frame_indices[i]);

    std::vector<ConstantModel> constant_models;
    std::vector<PointModel> point_models;
    std::vector<ExtendedModel> extended_models;

    auto source_iterator = group.begin();
    for (auto& source_model : source_models) {
      source_model->createParamsForFrame(frames[valid_frame_indices[i]]->getCoordinateSystem(), min_coord, first_frames[i]);
      double size_factor = 2;
      source_model->addModelsForFrame(i, extended_models, std::max(stamp_width, stamp_height)* size_factor);

      ++source_iterator;
    }

    std::cout << stamp_width << " " << stamp_height << "\n";

    // Full frame model with all sources
    FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model {
      pixel_scale,
      (size_t) stamp_width, (size_t) stamp_height,
      std::move(constant_models),
      std::move(point_models),
      std::move(extended_models),
      *psfs[i]
    };

    auto data_vs_model =
        createDataVsModelResiduals(image, std::move(frame_model), weight, LogChiSquareComparator{});
    res_estimator.registerBlockProvider(std::move(data_vs_model));

    images.emplace_back(image);
    weights.emplace_back(weight);
  }

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

//  if (solution.success_flag) {
//    std::cout << "Success!\n";
//  }

  for (auto& source_model : source_models) {
    std::cout << "After:  ";
    source_model->debugPrint();
  }

  printLevmarInfo(boost::any_cast<std::array<double,10>>(solution.underlying_framework_info));
  size_t iterations = (size_t) boost::any_cast<std::array<double,10>>(solution.underlying_framework_info)[5];

  double avg_reduced_chi_squared = 0;
  for (int i = 0; i < (int) valid_frame_indices.size(); i++) {
    auto stamp_width = images[i]->getWidth();
    auto stamp_height = images[i]->getHeight();
    //auto final_stamp = VectorImage<SeFloat>::create(stamp_width, stamp_height);

    std::vector<ExtendedModel> extended_models {};
    std::vector<PointModel> point_models {};
    std::vector<ConstantModel> constant_models;


    int nb_of_params = 0;
    for (auto& source_model : source_models) {
      source_model->addModelsForFrame(i, extended_models, std::max(stamp_width, stamp_height) * 2);
      nb_of_params += source_model->getNumberOfParameters();
    }

    FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model_after {
      pixel_scale,
      (size_t) stamp_width, (size_t) stamp_height,
      std::move(constant_models),
      std::move(point_models),
      std::move(extended_models),
      *psfs[i]
    };
    auto final_stamp = frame_model_after.getImage();

    for (int x=0; x<final_stamp->getWidth(); x++) {
      for (int y=0; y<final_stamp->getHeight(); y++) {
        debug_images[valid_frame_indices[i]]->at(x + min_coords[i].m_x, y + min_coords[i].m_y) += final_stamp->getValue(x,y);
      }
    }

    SeFloat reduced_chi_squared = computeReducedChiSquared(images[i], final_stamp, weights[i], nb_of_params);
    avg_reduced_chi_squared += reduced_chi_squared;
    printDebugChi2(reduced_chi_squared);
  }

  avg_reduced_chi_squared /= valid_frame_indices.size();

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

  {
    int i = 0;
    for (auto& debug_image : debug_images) {
      std::stringstream file_name;
      file_name << "debug_" << i << ".fits";

      FitsWriter::writeFile(*debug_image, file_name.str());

      auto frame = group.begin()->getProperty<MeasurementFrame>(i).getFrame();
      auto residual_image = SubtractImage<SeFloat>::create(frame->getOriginalImage(), debug_image);
      std::stringstream res_file_name;
      res_file_name << "res_" << i << ".fits";

      FitsWriter::writeFile(*residual_image, res_file_name.str());

      i++;
    }
  }

  std::cout << "...\n";
}


}
