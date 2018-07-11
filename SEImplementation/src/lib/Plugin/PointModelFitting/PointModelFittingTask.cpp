/*
 * PointModelFittingTask.cpp
 *
 *  Created on: July 11, 2018
 *      Author: Alejandro Álvarez
 */

#include "ModelFitting/Engine/DataVsModelResiduals.h"
#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Engine/LevmarEngine.h"
#include "ModelFitting/Engine/LogChiSquareComparator.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"
#include "ModelFitting/utils.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Image/ImagePsf.h"
#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFitting.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFittingTask.h"
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

using namespace ModelFitting;

namespace SExtractor {

namespace {

SeFloat computeReducedChiSquared(
  std::shared_ptr<const Image<SeFloat>> image, std::shared_ptr<const Image<SeFloat>> model, std::shared_ptr<const Image<SeFloat>> weights) {
  double reduced_chi_squared = 0.0;
  for (int y=0; y < image->getHeight(); y++) {
    for (int x=0; x < image->getWidth(); x++) {
      double tmp = image->getValue(x, y) - model->getValue(x, y);
      reduced_chi_squared += tmp * tmp * weights->getValue(x, y) * weights->getValue(x, y);
    }
  }

  return reduced_chi_squared / (image->getWidth() * image->getHeight());
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
  std::cout << "Med Reduced Chi^2: " << chi_squares[chi_squares.size() / 2] << "\n";
  std::cout << "90% Reduced Chi^2: " << chi_squares[chi_squares.size() * 9 / 10] << "\n";
}
}

struct SourceModel {
  EngineParameter dx, dy;
  DependentParameter<EngineParameter> x, y;
  EngineParameter flux;

  SourceModel(double x_guess, double y_guess, double pos_range, double flux_guess) :
    dx(0, make_unique<SigmoidConverter>(-pos_range, pos_range)),
    dy(0, make_unique<SigmoidConverter>(-pos_range, pos_range)),
    x([x_guess](double dx) { return dx + x_guess; }, dx),
    y([y_guess](double dy) { return dy + y_guess; }, dy),
    flux(flux_guess, make_unique<SigmoidConverter>(flux_guess * .00001, flux_guess * 20)) {}

  void registerParameters(EngineParameterManager& manager) {
    manager.registerParameter(dx);
    manager.registerParameter(dy);
    manager.registerParameter(flux);
  }

  void createModels(std::vector<PointModel>& point_models) {
    point_models.emplace_back(x, y, flux);
  }
};

void PointModelFittingTask::computeProperties(SExtractor::SourceGroupInterface &group) const {
  PixelCoordinate stamp_top_left = group.getProperty<DetectionFrameGroupStamp>().getTopLeft();
  auto &group_stamp = group.getProperty<DetectionFrameGroupStamp>().getStamp();
  auto &variance_stamp = group.getProperty<DetectionFrameGroupStamp>().getVarianceStamp();
  auto &thresholded_stamp = group.getProperty<DetectionFrameGroupStamp>().getThresholdedStamp();
  auto &group_psf = group.getProperty<PsfProperty>().getPsf();

  EngineParameterManager manager{};
  std::vector<ConstantModel> constant_models;
  std::vector<ExtendedModel> extended_models;
  std::vector<PointModel> point_models;
  std::vector<std::unique_ptr<SourceModel>> source_models;

  // Setup models for all the sources
  for (auto &source : group) {
    auto &pixel_centroid = source.getProperty<PixelCentroid>();
    auto &pixel_boundaries = source.getProperty<PixelBoundaries>();
    auto iso_flux = source.getProperty<IsophotalFlux>().getFlux();

    auto pos_range = group_psf.getSize();

    double guess_x = pixel_centroid.getCentroidX() - stamp_top_left.m_x;
    double guess_y = pixel_centroid.getCentroidY() - stamp_top_left.m_y;
    double flux_guess = iso_flux;

    source_models.push_back(make_unique<SourceModel>(guess_x, guess_y, pos_range, flux_guess));
    source_models.back()->createModels(point_models);
    source_models.back()->registerParameters(manager);
  }

  // Full frame model with all sources
  FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model{
    1,
    (size_t) group_stamp.getWidth(), (size_t) group_stamp.getHeight(),
    std::move(constant_models),
    std::move(point_models),
    std::move(extended_models),
    group_psf
  };

  auto weight = VectorImage<SeFloat>::create(group_stamp.getWidth(), group_stamp.getHeight());
  for (int y = 0; y < group_stamp.getHeight(); y++) {
    for (int x = 0; x < group_stamp.getWidth(); x++) {
      weight->at(x, y) = (thresholded_stamp.getValue(x, y) >= 0) ? 0 : 1;
    }
  }

  for (auto &source : group) {
    auto &pixel_coordinates = source.getProperty<PixelCoordinateList>().getCoordinateList();
    for (auto pixel : pixel_coordinates) {
      pixel -= stamp_top_left;
      weight->at(pixel.m_x, pixel.m_y) = 1;
    }
  }

  auto frame = group.begin()->getProperty<DetectionFrame>().getFrame();
  SeFloat gain = frame->getGain();
  SeFloat saturation = frame->getSaturation();

  for (int y = 0; y < group_stamp.getHeight(); y++) {
    for (int x = 0; x < group_stamp.getWidth(); x++) {
      auto back_var = variance_stamp.getValue(x, y);
      if (saturation > 0 && group_stamp.getValue(x, y) > saturation) {
        weight->at(x, y) = 0;
      } else if (weight->at(x, y) > 0) {
        if (gain > 0.0) {
          weight->at(x, y) = sqrt(1.0 / (back_var + group_stamp.getValue(x, y) / gain));
        } else {
          weight->at(x, y) = sqrt(1.0 / back_var); // infinite gain
        }
      }
    }
  }

  auto image = VectorImage<SeFloat>::create(group_stamp);

  auto data_vs_model =
    createDataVsModelResiduals(image, std::move(frame_model), weight, LogChiSquareComparator{});

  ResidualEstimator res_estimator{};
  res_estimator.registerBlockProvider(move(data_vs_model));

  // Perform the minimization
  LevmarEngine engine{m_max_iterations, 1E-6, 1E-6, 1E-6, 1E-6, 1E-4};

  auto solution = engine.solveProblem(manager, res_estimator);

  size_t iterations = (size_t) boost::any_cast<std::array<double, 10>>(solution.underlying_framework_info)[5];

  auto final_stamp = VectorImage<SeFloat>::create(group_stamp.getWidth(), group_stamp.getHeight());
  auto check_image = CheckImages::getInstance().getModelFittingCheckImage();

  auto source_iter = group.begin();
  for (auto &source_model : source_models) {
    auto &source = *source_iter;
    ++source_iter;

    // renders an image of the model for a single source with the final parameters
    std::vector<ConstantModel> constant_models{};
    std::vector<ExtendedModel> extended_models{};
    std::vector<PointModel> point_models{};
    source_model->createModels(point_models);
    FrameModel<ImagePsf, std::shared_ptr<VectorImage<SeFloat>>> frame_model_after{
      1, (size_t) group_stamp.getWidth(), (size_t) group_stamp.getHeight(),
      std::move(constant_models), std::move(point_models), std::move(extended_models),
      group_psf
    };

    auto final_image = frame_model_after.getImage();

    double total_flux = 0.;
    for (int y = 0; y < group_stamp.getHeight(); y++) {
      for (int x = 0; x < group_stamp.getWidth(); x++) {
        PixelCoordinate pixel(x, y);
        pixel += stamp_top_left;
        final_stamp->setValue(x, y, final_stamp->getValue(x, y) + final_image->getValue(x, y));
        if (check_image) {
          check_image->setValue(pixel.m_x, pixel.m_y, check_image->getValue(pixel) + final_image->getValue(x, y));
        }
        total_flux += final_image->getValue(x, y);
      }
    }

    auto coordinate_system = source.getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

    SeFloat x = stamp_top_left.m_x + source_model->x.getValue();
    SeFloat y = stamp_top_left.m_y + source_model->y.getValue();
    ImageCoordinate image_coordinate(x, y);
    auto world_coordinate = coordinate_system->imageToWorld(image_coordinate);

    source.setProperty<PointModelFitting>(
      x, y,
      world_coordinate.m_alpha, world_coordinate.m_delta,
      total_flux, iterations
    );
  }

  SeFloat reduced_chi_squared = computeReducedChiSquared(image, final_stamp, weight);
  printDebugChi2(reduced_chi_squared);
}

} // SExtractor