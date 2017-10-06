/*
 * MultiframeModelFittingTask.cpp
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#include <iostream>
#include <tuple>
#include <vector>
#include <valarray>
#include <boost/any.hpp>

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
#include "ModelFitting/Engine/ChiSquareComparator.h"


#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"


#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"

#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"

#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFitting.h"
#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeModelFittingTask.h"
#include "SEImplementation/Image/VectorImageDataVsModelInputTraits.h"


namespace SExtractor {

using namespace ModelFitting;

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

}

class SourceModel {

  // common parameters
  double m_radius_guess, m_exp_flux_guess, m_dev_flux_guess;

  EngineParameter dx, dy;

  ManualParameter exp_xs { 1 };
  ManualParameter exp_n { 1 };

  ManualParameter dev_xs { 1 };
  ManualParameter dev_n { 4 };

  EngineParameter exp_effective_radius;
  EngineParameter dev_effective_radius;

  DependentParameter<EngineParameter> exp_k;
  DependentParameter<EngineParameter> dev_k;

  std::vector<EngineParameter> exp_i0s;
  std::vector<EngineParameter> dev_i0s;

  std::vector<DependentParameter<EngineParameter>> pixel_x;
  std::vector<DependentParameter<EngineParameter>> pixel_y;

  ManualParameter exp_aspect {1}, exp_rot {0};
  ManualParameter dev_aspect {1}, dev_rot {0};

  //  EngineParameter exp_aspect, exp_rot;
//  EngineParameter dev_aspect, dev_rot;
//
//  // per measurement frame parameters
//


public:

  SourceModel(const SourceInterface& source) :
    m_exp_flux_guess(getFluxGuess(source)),
    m_dev_flux_guess(getFluxGuess(source)),
    m_radius_guess(getRadiusGuess(source)),

    dx(0, make_unique<SigmoidConverter>(-m_radius_guess, m_radius_guess)),
    dy(0, make_unique<SigmoidConverter>(-m_radius_guess, m_radius_guess)),

    exp_effective_radius(m_radius_guess, make_unique<ExpSigmoidConverter>(m_radius_guess * 0.001, m_radius_guess * 100)),
    dev_effective_radius(m_radius_guess, make_unique<ExpSigmoidConverter>(m_radius_guess * 0.001, m_radius_guess * 100)),

    exp_k(
        [](double eff_radius) { return 1.7 / eff_radius; },
        exp_effective_radius),
    dev_k(
        [](double eff_radius) { return pow(3459.0 / eff_radius, .25); },
        dev_effective_radius)
  {
  }

  void addModelsForFrame(std::vector<ExtendedModel>& extended_models, SeFloat source_x, SeFloat source_y, int size, PixelCoordinate offset) {
    auto exp_i0_guess = m_exp_flux_guess / (M_PI * 2.0 * 0.346 * m_radius_guess * m_radius_guess);
    exp_i0s.emplace_back(exp_i0_guess, make_unique<ExpSigmoidConverter>(exp_i0_guess * .00001, exp_i0_guess * 20));

    auto dev_i0_guess = m_dev_flux_guess * pow(10, 3.33) / (7.2 * M_PI * m_radius_guess * m_radius_guess);
    dev_i0s.emplace_back(dev_i0_guess, make_unique<ExpSigmoidConverter>(dev_i0_guess * .00001, dev_i0_guess * 20));


    std::cout << "0" << " " << source_x << " " << offset.m_x << "\n";

    pixel_x.emplace_back([source_x, offset](double dx) { return dx + source_x - offset.m_x; }, dx);
    pixel_y.emplace_back([source_y, offset](double dy) { return dy + source_y - offset.m_y; }, dy);

    // exponential
    {
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto exp = make_unique<SersicModelComponent>(make_unique<OnlySmooth>(), exp_i0s.back(), exp_n, exp_k);
      component_list.clear();
      component_list.emplace_back(std::move(exp));
      extended_models.emplace_back(std::move(component_list), exp_xs, exp_aspect, exp_rot, size, size, pixel_x.back(), pixel_y.back());
    }
    // devaucouleurs
    {
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto dev = make_unique<SersicModelComponent>(make_unique<AutoSharp>(), dev_i0s.back(), dev_n, dev_k);
      component_list.clear();
      component_list.emplace_back(std::move(dev));
      extended_models.emplace_back(std::move(component_list), dev_xs, dev_aspect, dev_rot, size, size, pixel_x.back(), pixel_y.back());
    }
  }

  void registerParameters(EngineParameterManager& manager) {
    manager.registerParameter(dx);
    manager.registerParameter(dy);

    manager.registerParameter(exp_effective_radius);
    manager.registerParameter(dev_effective_radius);

    for (auto& exp_i0 : exp_i0s) {
      manager.registerParameter(exp_i0);
    }

    for (auto& dev_i0 : dev_i0s) {
      manager.registerParameter(dev_i0);
    }
  }

  void debugPrint() const {
    //std::cout << "\tsize: " << m_size << "\n";
    std::cout << "\tx: " << dx.getValue() << "\ty: " << dy.getValue() << "\n";
    //std::cout << "\texp i0: " << exp_i0.getValue()<< "\tReff: " << exp_effective_radius.getValue() << "\n";
    //std::cout << "\tdev i0: " << dev_i0.getValue()<< "\tReff: " << dev_effective_radius.getValue() << "\n";
  }

  double getFluxGuess(const SourceInterface& source) const {
    auto iso_flux = source.getProperty<IsophotalFlux>().getFlux();
    return iso_flux / 2;
  }

  double getRadiusGuess(const SourceInterface& source) const {
    return 5; //FIXME
  }

};

MultiframeModelFittingTask::MultiframeModelFittingTask(std::shared_ptr<ImagePsf> psf, unsigned int max_iterations)
  : m_psf(psf),
    m_max_iterations(max_iterations)
{
  m_frame_indices.push_back(0); // FIXME for test use the detection image only
}

void MultiframeModelFittingTask::computeProperties(SourceGroupInterface& group) const {
  //auto& group_stamp = group.getProperty<MeasurementFrameGroupStamp>().getStamp();

  double pixel_scale = 1;
  auto& group_stamp = group.getProperty<DetectionFrameGroupStamp>().getStamp();

  EngineParameterManager manager {};
  std::vector<std::unique_ptr<SourceModel>> source_models;

  std::vector<std::shared_ptr<MeasurementImageFrame>> frames;
  for (auto frame_index : m_frame_indices) {
    frames.push_back(group.begin()->getProperty<MeasurementFrame>(frame_index).getFrame());
  }

  // Setup models for all the sources
  for (auto& source : group) {
    source_models.emplace_back(std::unique_ptr<SourceModel>(new SourceModel(source)));
  }

  ResidualEstimator res_estimator {};

  for (auto& frame_index : m_frame_indices) {

    std::cout << "frame\n";

    PixelCoordinate min_coord(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    PixelCoordinate max_coord(std::numeric_limits<int>::lowest(), std::numeric_limits<int>::lowest());

    for (auto& source : group) {
      auto& frame_centroid = source.getProperty<MeasurementFramePixelCentroid>(frame_index);
      auto frame_x = (int) frame_centroid.getCentroidX();
      auto frame_y = (int) frame_centroid.getCentroidY();

      min_coord.m_x = std::min(min_coord.m_x, frame_x);
      min_coord.m_y = std::min(min_coord.m_y, frame_y);
      max_coord.m_x = std::max(max_coord.m_x, frame_x);
      max_coord.m_y = std::max(max_coord.m_y, frame_y);
    }

    auto frame_image = frames[frame_index]->getSubtractedImage();

    // FIXME fixed 10 pixel border for now
    min_coord.m_x = std::max(0, min_coord.m_x - 10);
    min_coord.m_y = std::max(0, min_coord.m_y - 10);
    max_coord.m_x = std::min(frame_image->getWidth()-1, max_coord.m_x + 10);
    max_coord.m_y = std::min(frame_image->getHeight()-1, max_coord.m_y + 10);

    int stamp_width = max_coord.m_x - min_coord.m_x;
    int stamp_height = max_coord.m_y - min_coord.m_y;

    auto image = VectorImage<SeFloat>::create(stamp_width, stamp_height);
    for (int y=0; y<stamp_height; y++) {
      for (int x=0; x<stamp_width; x++) {
        image->at(x, y) = frame_image->getValue(min_coord.m_x + x, min_coord.m_y + y);
      }
    }

    auto weight = VectorImage<SeFloat>::create(stamp_width, stamp_height);
    std::fill(weight->getData().begin(), weight->getData().end(), 1);

    std::vector<ConstantModel> constant_models;
    std::vector<PointModel> point_models;
    std::vector<ExtendedModel> extended_models;

    auto source_iterator = group.begin();
    for (auto& source_model : source_models) {

      std::cout << "source_model\n";
      auto& frame_centroid = source_iterator->getProperty<MeasurementFramePixelCentroid>(frame_index);
      auto frame_x = frame_centroid.getCentroidX();
      auto frame_y = frame_centroid.getCentroidY();

      source_model->addModelsForFrame(extended_models, frame_x, frame_y, std::max(stamp_width, stamp_height), min_coord);

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
      *m_psf
    };

    auto data_vs_model =
        createDataVsModelResiduals(image, std::move(frame_model), weight, LogChiSquareComparator{});
    res_estimator.registerBlockProvider(move(data_vs_model));
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


  for (auto& source : group) {
    source.setProperty<MultiframeModelFitting>(
        99, 99,
        99, 99,
        99, 99
//        total_flux, iterations,
//        0,0,0,0
        );
  }
}

}
