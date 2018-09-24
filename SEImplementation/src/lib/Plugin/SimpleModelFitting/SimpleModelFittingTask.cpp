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
#include <SEImplementation/Plugin/Psf/PsfProperty.h>
#include <mutex>

#include "ElementsKernel/PathSearch.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Image/ImagePsf.h"

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
#include "ModelFitting/utils.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LevmarEngine.h"

#include "ModelFitting/Engine/LogChiSquareComparator.h"
#include "ModelFitting/Engine/ChiSquareComparator.h"


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
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"



namespace SExtractor {

using namespace ModelFitting;

/////////// FIXME copypasta from ModelFitting tests, move somewhere else

namespace {

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

SeFloat computeReducedChiSquared(std::shared_ptr<const Image<SeFloat>> image,
    std::shared_ptr<const Image<SeFloat>> model, std::shared_ptr<const Image<SeFloat>> weights) {
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

///////////////////////////////////////////////////////////////////////////////


struct SourceModel {
  double m_size;
  EngineParameter dx, dy;
  DependentParameter<EngineParameter> x, y;

  //double exp_i0_guess;
  //EngineParameter exp_i0, exp_effective_radius, exp_aspect, exp_rot;
  EngineParameter exp_flux, exp_effective_radius, exp_aspect, exp_rot;
  //double dev_i0_guess;
  EngineParameter dev_flux, dev_effective_radius, dev_aspect, dev_rot;

  ManualParameter exp_xs { 1 };
  ManualParameter exp_n { 1 };

  ManualParameter dev_xs { 1 };
  ManualParameter dev_n { 4 };

  DependentParameter<EngineParameter> exp_k;
  DependentParameter<EngineParameter> dev_k;

  DependentParameter<EngineParameter, EngineParameter, EngineParameter> exp_i0;
  DependentParameter<EngineParameter, EngineParameter, EngineParameter> dev_i0;

  SourceModel(double size, double x_guess, double y_guess, double pos_range,
      double exp_flux_guess, double exp_radius_guess, double exp_aspect_guess, double exp_rot_guess,
      double dev_flux_guess, double dev_radius_guess, double dev_aspect_guess, double dev_rot_guess) :

    m_size(size),
    dx(0, make_unique<SigmoidConverter>(-pos_range, pos_range)),
    dy(0, make_unique<SigmoidConverter>(-pos_range, pos_range)),

    x([x_guess](double dx) { return dx + x_guess; }, dx),
    y([y_guess](double dy) { return dy + y_guess; }, dy),

    //exp_i0_guess(exp_flux_guess / (M_PI * 2.0 * 0.346 * exp_radius_guess * exp_radius_guess * exp_aspect_guess)),
    //exp_i0(exp_i0_guess, make_unique<ExpSigmoidConverter>(exp_i0_guess * .00001, exp_i0_guess * 20)),
    exp_flux(exp_flux_guess, make_unique<ExpSigmoidConverter>(exp_flux_guess * .00001, exp_flux_guess * 20)),
    exp_effective_radius(exp_radius_guess, make_unique<ExpSigmoidConverter>(0.01, exp_radius_guess * 10)),
    exp_aspect(exp_aspect_guess, make_unique<SigmoidConverter>(0.01, 1.0)),
    exp_rot(-exp_rot_guess, make_unique<SigmoidConverter>(-M_PI, M_PI)),

    //dev_i0_guess(dev_flux_guess * pow(10, 3.33) / (7.2 * M_PI * dev_radius_guess * dev_radius_guess * dev_aspect_guess)),
    //dev_i0(dev_i0_guess, make_unique<ExpSigmoidConverter>(dev_i0_guess * .00001, dev_i0_guess * 20)),
    dev_flux(dev_flux_guess, make_unique<ExpSigmoidConverter>(dev_flux_guess * .00001, dev_flux_guess * 20)),
    dev_effective_radius(dev_radius_guess, make_unique<ExpSigmoidConverter>(0.01, dev_radius_guess * 10)),
    dev_aspect(dev_aspect_guess, make_unique<SigmoidConverter>(0.01, 1.0)),
    dev_rot(-dev_rot_guess, make_unique<SigmoidConverter>(-M_PI, M_PI)),

    exp_k(
        [](double eff_radius) { return 1.7 / eff_radius; },
        exp_effective_radius),

    dev_k(
        [](double eff_radius) { return pow(3459.0 / eff_radius, .25); },
        dev_effective_radius),

    exp_i0(
        [](double flux, double radius, double aspect) { return flux / (M_PI * 2.0 * 0.346 * radius * radius * aspect); },
        exp_flux, exp_effective_radius, exp_aspect),
    dev_i0(
        [](double flux, double radius, double aspect) { return flux * pow(10, 3.33) / (7.2 * M_PI * radius * radius *  aspect); },
        dev_flux, dev_effective_radius, dev_aspect)
  {
  }

  void registerParameters(EngineParameterManager& manager) {
    manager.registerParameter(dx);
    manager.registerParameter(dy);

//    manager.registerParameter(exp_i0);
    manager.registerParameter(exp_flux);
    manager.registerParameter(exp_effective_radius);
    manager.registerParameter(exp_aspect);
    manager.registerParameter(exp_rot);

    manager.registerParameter(dev_flux);
    manager.registerParameter(dev_effective_radius);
    manager.registerParameter(dev_aspect);
    manager.registerParameter(dev_rot);
  }

  void createModels(std::vector<TransformedModel>& extended_models, std::vector<PointModel>& /*point_models*/) {
    // exponential
    {
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto exp = make_unique<SersicModelComponent>(make_unique<OldSharp>(), exp_i0, exp_n, exp_k);
      component_list.clear();
      component_list.emplace_back(std::move(exp));
      extended_models.emplace_back(std::move(component_list), exp_xs, exp_aspect, exp_rot, m_size, m_size, x, y);
    }
    // devaucouleurs
    {
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto dev = make_unique<SersicModelComponent>(make_unique<OldSharp>(), dev_i0, dev_n, dev_k);
      component_list.clear();
      component_list.emplace_back(std::move(dev));
      extended_models.emplace_back(std::move(component_list), dev_xs, dev_aspect, dev_rot, m_size, m_size, x, y);
    }
  }

  double getExpFlux() const {
    auto exp_radius = exp_effective_radius.getValue();
    return exp_i0.getValue() * (M_PI * 2.0 * 0.346 * exp_radius * exp_radius * exp_aspect.getValue());
  }

  double getDevFlux() const {
    auto dev_radius = dev_effective_radius.getValue();
    return dev_i0.getValue() * (7.2 * M_PI * dev_radius * dev_radius * dev_aspect.getValue()) / pow(10, 3.33);
  }

  double getTotalFlux() const {
    return getExpFlux() + getDevFlux();
  }

  double getExpRatio() const {
    return exp_aspect.getValue();
  }

  double getExpAngle() const {
    return exp_rot.getValue();
  }

  double getDevRatio() const {
    return dev_aspect.getValue();
  }

  double getDevAngle() const {
    return dev_rot.getValue();
  }

  double getExpRadius() const {
    return exp_effective_radius.getValue();
  }

  double getDevRadius() const {
    return dev_effective_radius.getValue();
  }

  void debugPrint() const {
    std::cout << "\tsize: " << m_size << "\n";
    std::cout << "\tx: " << x.getValue() << "\ty: " << y.getValue() << "\n";
    std::cout << "\texp i0: " << exp_i0.getValue()<< "\tReff: " << exp_effective_radius.getValue() << "\n";
    std::cout << "\tdev i0: " << dev_i0.getValue()<< "\tReff: " << dev_effective_radius.getValue() << "\n";
  }
};

}


void SimpleModelFittingTask::computeProperties(SourceGroupInterface& group) const {
  auto& group_stamp = group.getProperty<DetectionFrameGroupStamp>().getStamp();
  auto& thresholded_stamp = group.getProperty<DetectionFrameGroupStamp>().getThresholdedStamp();
  auto& variance_stamp = group.getProperty<DetectionFrameGroupStamp>().getVarianceStamp();
  //auto& weight_stamp = group.getProperty<DetectionFrameGroupStamp>().getWeightStamp();
  PixelCoordinate stamp_top_left = group.getProperty<DetectionFrameGroupStamp>().getTopLeft();
  auto group_psf = group.getProperty<PsfProperty>().getPsf();

  double pixel_scale = 1;

  EngineParameterManager manager {};
  std::vector<ConstantModel> constant_models;
  std::vector<TransformedModel> extended_models;
  std::vector<PointModel> point_models;
  std::vector<std::unique_ptr<SourceModel>> source_models;

  // Setup models for all the sources
  for (auto& source : group) {
    auto& pixel_centroid = source.getProperty<PixelCentroid>();
    auto& shape_parameters = source.getProperty<ShapeParameters>();
    auto& pixel_boundaries = source.getProperty<PixelBoundaries>();
    auto iso_flux = source.getProperty<IsophotalFlux>().getFlux();

    double size_factor = 2;
    auto size = std::max<double>(pixel_boundaries.getWidth(), pixel_boundaries.getHeight()) * size_factor;

    //double radius_guess = std::max<double>(half_maximum_boundaries.getWidth(), half_maximum_boundaries.getHeight()) / 2.0;
    double radius_guess = shape_parameters.getEllipseA() / 2.0;
    //std::cout << radius_guess << "....\n";

    double guess_x = pixel_centroid.getCentroidX() - stamp_top_left.m_x;
    double guess_y = pixel_centroid.getCentroidY() - stamp_top_left.m_y;
    double exp_flux_guess = iso_flux / 2.0;
    double exp_reff_guess = radius_guess;
    double exp_aspect_guess = std::max<double>(shape_parameters.getEllipseB() / shape_parameters.getEllipseA(), 0.01);
    double exp_rot_guess = shape_parameters.getEllipseTheta();
    double dev_flux_guess = iso_flux / 2.0;
    double dev_reff_guess = radius_guess;
    double dev_aspect_guess = exp_aspect_guess;
    double dev_rot_guess = exp_rot_guess;

    source_models.push_back(make_unique<SourceModel>(size, guess_x, guess_y, radius_guess * 2,
        exp_flux_guess, exp_reff_guess, exp_aspect_guess, exp_rot_guess,
        dev_flux_guess, dev_reff_guess, dev_aspect_guess, dev_rot_guess));

    source_models.back()->createModels(extended_models, point_models);
    source_models.back()->registerParameters(manager);
  }

  // Full frame model with all sources
  FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model {
    pixel_scale,
    (size_t) group_stamp.getWidth(), (size_t) group_stamp.getHeight(),
    std::move(constant_models),
    std::move(point_models),
    std::move(extended_models),
    group_psf
  };

  auto weight = VectorImage<SeFloat>::create(group_stamp.getWidth(), group_stamp.getHeight());
  //std::fill(weight->getData().begin(), weight->getData().end(), 1); // FIXME test

  for (int y=0; y < group_stamp.getHeight(); y++) {
    for (int x=0; x < group_stamp.getWidth(); x++) {
      weight->at(x, y) = (thresholded_stamp.getValue(x, y) >= 0) ? 0 : 1;
    }
  }

  for (auto& source : group) {
    auto& pixel_coordinates = source.getProperty<PixelCoordinateList>().getCoordinateList();
    for (auto pixel : pixel_coordinates) {
      pixel -= stamp_top_left;
      weight->at(pixel.m_x, pixel.m_y) = 1;
    }
  }

  auto frame = group.begin()->getProperty<DetectionFrame>().getFrame();
  SeFloat gain = frame->getGain();
  SeFloat saturation = frame->getSaturation();

  for (int y=0; y < group_stamp.getHeight(); y++) {
    for (int x=0; x < group_stamp.getWidth(); x++) {
      auto back_var = variance_stamp.getValue(x, y);
      if (saturation > 0 && group_stamp.getValue(x, y) > saturation) {
        weight->at(x, y) = 0;
      } else if (weight->at(x, y)>0) {
        if (gain > 0.0) {
          weight->at(x, y) = sqrt(1.0 / (back_var + group_stamp.getValue(x, y) / gain));
        } else {
          weight->at(x, y) = sqrt(1.0 / back_var); // infinite gain
        }
      }
    }
  }

  // FIXME we should be able to use the group_stamp Image interface directly
  auto image = VectorImage<SeFloat>::create(group_stamp);

  auto data_vs_model =
      createDataVsModelResiduals(image, std::move(frame_model), weight, LogChiSquareComparator{});

  ResidualEstimator res_estimator {};
  res_estimator.registerBlockProvider(move(data_vs_model));

  // Perform the minimization

  //LevmarEngine engine {m_max_iterations, 1E-3, 1E-6, 1E-6, 1E-6, 1E-4};
  LevmarEngine engine {m_max_iterations, 1E-6, 1E-6, 1E-6, 1E-6, 1E-4};

//  for (auto& source_model : source_models) {
//    std::cout << "Before: ";
//    source_model->debugPrint();
//  }

  auto solution = engine.solveProblem(manager, res_estimator);

//  for (auto& source_model : source_models) {
//    std::cout << "After: ";
//    source_model->debugPrint();
//  }
//  printLevmarInfo(boost::any_cast<std::array<double,10>>(solution.underlying_framework_info));

  size_t iterations = (size_t) boost::any_cast<std::array<double,10>>(solution.underlying_framework_info)[5];

  auto final_stamp = VectorImage<SeFloat>::create(group_stamp.getWidth(), group_stamp.getHeight());
  auto check_image = CheckImages::getInstance().getModelFittingCheckImage();
  auto source_iter = group.begin();
  for (auto& source_model : source_models) {
    auto& source = *source_iter;
    ++source_iter;

    // renders an image of the model for a single source with the final parameters
    std::vector<TransformedModel> extended_models {};
    std::vector<PointModel> point_models {};
    source_model->createModels(extended_models, point_models);
    FrameModel<ImagePsf, std::shared_ptr<VectorImage<SeFloat>>> frame_model_after {
      1, (size_t) group_stamp.getWidth(), (size_t) group_stamp.getHeight(), std::move(constant_models), std::move(point_models),
          std::move(extended_models), group_psf
    };
    auto final_image = frame_model_after.getImage();

    // integrates the flux for that source
    double total_flux = 0;
    for (int y=0; y < group_stamp.getHeight(); y++) {
      for (int x=0; x < group_stamp.getWidth(); x++) {
        PixelCoordinate pixel(x, y);
        pixel += stamp_top_left;

        // build final stamp
        final_stamp->setValue(x, y, final_stamp->getValue(x, y) + final_image->getValue(x, y));

        // if requested, updates a check image made by adding all source models
        if (check_image) {
          CheckImages::getInstance().lock();
          check_image->setValue(pixel.m_x, pixel.m_y, check_image->getValue(pixel) + final_image->getValue(x, y));
          CheckImages::getInstance().unlock();
        }

        total_flux += final_image->getValue(x, y);
      }
    }

    auto coordinate_system = source.getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

    SeFloat x = stamp_top_left.m_x + source_model->x.getValue();
    SeFloat y = stamp_top_left.m_y + source_model->y.getValue();
    ImageCoordinate image_coordinate(x, y);
    auto world_coordinate = coordinate_system->imageToWorld(image_coordinate);

    source.setProperty<SimpleModelFitting>(
        x, y,
        world_coordinate.m_alpha, world_coordinate.m_delta,
        total_flux, source_model->getExpFlux(), source_model->getDevFlux(),
        source_model->getExpRatio(), source_model->getExpAngle(), source_model->getExpRadius(),
        source_model->getDevRatio(), source_model->getDevAngle(), source_model->getDevRadius(),
        iterations
    );
  }

  //SeFloat reduced_chi_squared = computeReducedChiSquared(image, final_stamp, weight);
  //printDebugChi2(reduced_chi_squared);
}

}

