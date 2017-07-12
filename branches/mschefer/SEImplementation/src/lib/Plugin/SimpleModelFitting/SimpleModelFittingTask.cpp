/*
 * SimpleModelTask.cpp
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#include <iostream>
#include <tuple>
#include <vector>
#include <valarray>
#include <boost/any.hpp>

#include <CCfits/CCfits>

#include "ElementsKernel/PathSearch.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"
#include "ModelFitting/Parameters/NormalizedConverter.h"
#include "ModelFitting/Parameters/NormalizedPositiveConverter.h"
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/OldSharp.h"
#include "ModelFitting/Models/AutoSharp.h"
#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Image/OpenCvMatImageTraits.h"
#include "ModelFitting/utils.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LevmarEngine.h"
#include "ModelFitting/Image/OpenCvPsf.h"

#include "ModelFitting/Engine/OpenCvDataVsModelInputTraits.h"

#include "ModelFitting/Engine/LogChiSquareComparator.h"


#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"

#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFitting.h"
#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFittingTask.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"

#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEFramework/Property/DetectionFrame.h"


namespace SExtractor {

using namespace ModelFitting;

/////////// FIXME copypasta from ModelFitting tests, move somewhere else

// Prints on the screen the info of the levmar minimization
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

SeFloat computeReducedChiSquared(const cv::Mat& image, cv::Mat& model, cv::Mat& weights) {
  double reduced_chi_squared = 0.0;
  for (int y=0; y < image.rows; y++) {
    for (int x=0; x < image.cols; x++) {
      double tmp = image.at<double>(y, x) - model.at<double>(y, x);
      reduced_chi_squared += tmp * tmp * weights.at<double>(y, x) * weights.at<double>(y, x);
    }
  }

  return reduced_chi_squared / (image.rows * image.cols);
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

///////////////////////////////////////////////////////////////////////////////

struct SourceModel {
  double size;
  EngineParameter x, y;
//  EngineParameter x_scale, y_scale, rot_angle, i0;
//  EngineParameter dev_x_scale, dev_y_scale, dev_rot_angle, dev_i0;
  EngineParameter x_scale, y_scale, rot_angle, i0, k;
  EngineParameter dev_x_scale, dev_y_scale, dev_rot_angle, dev_i0, dev_k;

  void registerParameters(EngineParameterManager& manager) {
    manager.registerParameter(x);
    manager.registerParameter(y);

    manager.registerParameter(x_scale);
    manager.registerParameter(y_scale);
    manager.registerParameter(rot_angle);
    manager.registerParameter(i0);
    manager.registerParameter(k);

    manager.registerParameter(dev_x_scale);
    manager.registerParameter(dev_y_scale);
    manager.registerParameter(dev_rot_angle);
    manager.registerParameter(dev_i0);
    manager.registerParameter(dev_k);
  }

  void createModels(std::vector<ExtendedModel>& extended_models, std::vector<PointModel>& point_models, bool test = false) {

//    point_models.emplace_back(x, y, i0);
    //ManualParameter k { 1 };

    std::vector<std::unique_ptr<ModelComponent>> component_list {};

    // exponential
    ManualParameter exp_n { 1 };
    auto exp = make_unique<SersicModelComponent>(make_unique<OnlySmooth>(), i0, exp_n, k);
    component_list.clear();
    component_list.emplace_back(std::move(exp));
    extended_models.emplace_back(std::move(component_list), x_scale, y_scale, rot_angle, size, size, x, y);

    // devaucouleur
//    if(!test){
    ManualParameter devaucouleur_n { 4 };
    auto dev = make_unique<SersicModelComponent>(make_unique<OldSharp>(), dev_i0, devaucouleur_n, dev_k);
    component_list.clear();
    component_list.emplace_back(std::move(dev));
    extended_models.emplace_back(std::move(component_list), dev_x_scale, dev_y_scale,
        dev_rot_angle, size, size, x, y);
//}
  }

  void debugPrint() const {
    std::cout << "\tsize: " << size << "\n";
    std::cout << "\tx: " << x.getValue() << "\ty: " << y.getValue() << "\n";
    std::cout << "\txs: " << x_scale.getValue() << "\tys: " << y_scale.getValue() << "\trot: " << rot_angle.getValue()<< "\ti0: " << i0.getValue()<< "\tk: " << k.getValue() << "\n";
    std::cout << "dev\txs: " << dev_x_scale.getValue() << "\tys: " << dev_y_scale.getValue() << "\trot: " << dev_rot_angle.getValue()<< "\ti0: " << dev_i0.getValue()<< "\tk: " << dev_k.getValue() << "\n";
//    std::cout << "\txs: " << x_scale.getValue() << "\tys: " << y_scale.getValue() << "\trot: " << rot_angle.getValue()<< "\ti0: " << i0.getValue()<< "\n";
//    std::cout << "dev\txs: " << dev_x_scale.getValue() << "\tys: " << dev_y_scale.getValue() << "\trot: " << dev_rot_angle.getValue()<< "\ti0: " << dev_i0.getValue()<< "\n";
  }
};

void SimpleModelFittingTask::computeProperties(SourceGroupInterface& group) const {

  auto& group_stamp = group.getProperty<DetectionFrameGroupStamp>().getStamp();
  auto& thresholded_stamp = group.getProperty<DetectionFrameGroupStamp>().getThresholdedStamp();
  //auto& weight_stamp = group.getProperty<DetectionFrameGroupStamp>().getStamp();
  PixelCoordinate stamp_top_left = group.getProperty<DetectionFrameGroupStamp>().getTopLeft();

  double pixel_scale = 1;

  EngineParameterManager manager {};
  std::vector<ConstantModel> constant_models;
  std::vector<ExtendedModel> extended_models;
  std::vector<PointModel> point_models;
  std::vector<SourceModel> source_models;

  // Setup models for all the sources
  for (auto& source : group) {
    auto& pixel_centroid = source.getProperty<PixelCentroid>();
    auto& shape_parameters = source.getProperty<ShapeParameters>();
    auto& pixel_boundaries = source.getProperty<PixelBoundaries>();
    auto& half_maximum_boundaries = source.getProperty<PixelBoundariesHalfMaximum>();
    auto peak_value = source.getProperty<PeakValue>().getMaxValue();
    auto iso_flux = source.getProperty<IsophotalFlux>().getFlux();

    double size_factor = 2;
    auto size = std::max<double>(pixel_boundaries.getWidth(), pixel_boundaries.getHeight()) * size_factor;

    double radius_guess = std::max<double>(half_maximum_boundaries.getWidth(), half_maximum_boundaries.getHeight()) / 2.0;

    std::cout << radius_guess << "  :(\n";

//    double exp_scale = radius_guess / .693;
//    double dev_scale = radius_guess / .231;

    double exp_scale = 1;
    double dev_scale = 1;
    double exp_k_guess =  2.0 * log(.5) / -radius_guess;
    double dev_k_guess = 16.0 * log(.5) / -pow(radius_guess, .25);

    double exp_i0_guess = peak_value / 2.0;
    double dev_i0_guess = 10000 * peak_value / 2.0;

    // Creates the SourceModel with initial values
    SourceModel source_model {
      size,

      {pixel_centroid.getCentroidX() - stamp_top_left.m_x, make_unique<SigmoidConverter>(
          pixel_boundaries.getMin().m_x - stamp_top_left.m_x, pixel_boundaries.getMax().m_x - stamp_top_left.m_x + 1.0, 100)},
      {pixel_centroid.getCentroidY() - stamp_top_left.m_y, make_unique<SigmoidConverter>(
          pixel_boundaries.getMin().m_y - stamp_top_left.m_y, pixel_boundaries.getMax().m_y - stamp_top_left.m_y + 1.0, 100)},

      {exp_scale, make_unique<ExpSigmoidConverter>(0.01 * exp_scale, 10.0 * exp_scale)},
      {exp_scale, make_unique<ExpSigmoidConverter>(0.01 * exp_scale, 10.0 * exp_scale)},
      {shape_parameters.getEllipseTheta(), make_unique<SigmoidConverter>(-2*M_PI, 2*M_PI)},
      {exp_i0_guess, make_unique<ExpSigmoidConverter>(exp_i0_guess * .00001,exp_i0_guess * 100)},
      {exp_k_guess, make_unique<ExpSigmoidConverter>(exp_k_guess * 0.1, exp_k_guess * 100)},

      {dev_scale, make_unique<ExpSigmoidConverter>(0.01 * dev_scale, 10.0 * dev_scale)},
      {dev_scale, make_unique<ExpSigmoidConverter>(0.01 * dev_scale, 10.0 * dev_scale)},
      {shape_parameters.getEllipseTheta(), make_unique<SigmoidConverter>(-2*M_PI, 2*M_PI)},
      {dev_i0_guess, make_unique<ExpSigmoidConverter>(dev_i0_guess * .00001,dev_i0_guess * 10000000)},
      {dev_k_guess, make_unique<ExpSigmoidConverter>(dev_k_guess * 0.1, dev_k_guess * 100)},
    };

    source_model.createModels(extended_models, point_models);
    source_model.registerParameters(manager);
    source_models.push_back(source_model);
  }

//  // We add a constant background
//  ManualParameter back {0.};
//  constant_models.emplace_back(back);

  // Full frame model with all sources
  FrameModel<OpenCvPsf, cv::Mat> frame_model {
    pixel_scale,
    (size_t) group_stamp.getWidth(), (size_t) group_stamp.getHeight(),
    std::move(constant_models),
    std::move(point_models),
    std::move(extended_models),
    *m_psf
  };

  cv::Mat weight = cv::Mat::ones(group_stamp.getHeight(), group_stamp.getWidth(), CV_64F);

  for (int y=0; y < group_stamp.getHeight(); y++) {
    for (int x=0; x < group_stamp.getWidth(); x++) {

      if (thresholded_stamp.getValue(x, y) >= 0) {
        weight.at<double>(y, x) = 0;
      }
    }
  }
  for (auto& source : group) {
    auto& pixel_coordinates = source.getProperty<PixelCoordinateList>().getCoordinateList();
    for (auto pixel : pixel_coordinates) {
      pixel -= stamp_top_left;
      weight.at<double>(pixel.m_y, pixel.m_x) = 1;
    }
  }

  for (int y=0; y < group_stamp.getHeight(); y++) {
    for (int x=0; x < group_stamp.getWidth(); x++) {

      //FIXME hardcoded quick n dirty test for ignoring saturated pixels
      if (group_stamp.getValue(x, y)>64000) {
        weight.at<double>(y, x) = 0;
      } else   if (weight.at<double>(y, x)>0) {
        weight.at<double>(y, x) = sqrt(1.0 / (400.0 /*fixme background variance*/ + group_stamp.getValue(x, y) /*/ gain*/ ));

      }
    }
  }

  // FIXME for now just copy to an opencv image
  cv::Mat image (group_stamp.getHeight(), group_stamp.getWidth(), CV_64F);
  for (int y=0; y < group_stamp.getHeight(); y++) {
    for (int x=0; x < group_stamp.getWidth(); x++) {
      image.at<double>(y, x) = group_stamp.getValue(x, y);
    }
  }

  auto data_vs_model =
      createDataVsModelResiduals(image, std::move(frame_model), std::move(weight), LogChiSquareComparator{});

  ResidualEstimator res_estimator {};
  res_estimator.registerBlockProvider(move(data_vs_model));

  // Perform the minimization

  LevmarEngine engine {m_max_iterations};

  for (auto& source_model : source_models) {
    std::cout << "Before: ";
    source_model.debugPrint();
  }

  auto solution = engine.solveProblem(manager, res_estimator);

//  if (solution.success_flag) {
//    std::cout << "Success!\n";
//  }

  for (auto& source_model : source_models) {
    std::cout << "After:  ";
    source_model.debugPrint();
  }

  printLevmarInfo(boost::any_cast<std::array<double,10>>(solution.underlying_framework_info));

  size_t iterations = (size_t) boost::any_cast<std::array<double,10>>(solution.underlying_framework_info)[5];

  auto check_image = CheckImages::getInstance().getModelFittingCheckImage();
  auto source_iter = group.begin();
  for (auto& source_model : source_models) {
    auto& source = *source_iter;
    ++source_iter;

    // renders an image of the model for a single source with the final parameters
    std::vector<ExtendedModel> extended_models {};
    std::vector<PointModel> point_models {};
    source_model.createModels(extended_models, point_models, true);
    FrameModel<OpenCvPsf, cv::Mat> frame_model_after {
      1, (size_t) group_stamp.getWidth(), (size_t) group_stamp.getHeight(), std::move(constant_models), std::move(point_models),
          std::move(extended_models), *m_psf
    };
    auto final_image = frame_model_after.getImage();

    SeFloat reduced_chi_squared = computeReducedChiSquared(image, final_image, weight);
    printDebugChi2(reduced_chi_squared);

    // integrates the flux for that source
    double total_flux = 0;
    for (int y=0; y < group_stamp.getHeight(); y++) {
      for (int x=0; x < group_stamp.getWidth(); x++) {
        PixelCoordinate pixel (x,y);
        pixel += stamp_top_left;

        // if requested, updates a check image made by adding all source models
        if (check_image) {
          check_image->setValue(pixel, check_image->getValue(pixel) + final_image.at<double>(y, x));
        }

        total_flux += final_image.at<double>(y, x);
      }
    }

    auto coordinate_system = source.getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

    SeFloat x = stamp_top_left.m_x + source_model.x.getValue();
    SeFloat y = stamp_top_left.m_y + source_model.y.getValue();
    ImageCoordinate image_coordinate(x, y);
    auto world_coordinate = coordinate_system->imageToWorld(image_coordinate);

    source.setProperty<SimpleModelFitting>(
        x, y,
        world_coordinate.m_alpha, world_coordinate.m_delta,
        total_flux, iterations,
        source_model.y_scale.getValue() / source_model.x_scale.getValue(), source_model.rot_angle.getValue(),
        source_model.dev_y_scale.getValue() / source_model.dev_x_scale.getValue(), source_model.dev_rot_angle.getValue()
        );
  }
}

}


//    for (auto& source : group) {
//      auto& pixels =  source.getProperty<PixelCoordinateList>().getCoordinateList();
//      for (auto pixel : pixels) {
//        pixel -= stamp_top_left;
//        weight.at<double>(pixel.m_y, pixel.m_x) = 1;
//      }
//    }
