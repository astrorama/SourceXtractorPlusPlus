/** 
 * @file Example3.cpp
 * @date September 2, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include <tuple>
#include <vector>
#include "ElementsKernel/PathSearch.h"
#include "ModelFitting/Image/OpenCvMatImageTraits.h"
#include "ModelFitting/Image/OpenCvPsf.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"
#include "ModelFitting/Parameters/NormalizedConverter.h"
#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Engine/OpenCvDataVsModelInputTraits.h"
#include "ModelFitting/Engine/LogChiSquareComparator.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LevmarEngine.h"
#include "utils.h"
#include "ModelFitting/Parameters/NeutralConverter.h"

using namespace std;
using namespace ModelFitting;

// This example demonstrates how to use the DataVsModelResiduals to perform
// minimization over an observed image and a FrameModel. The real parameters
// are:
// - I0 : 12.
// - X : 128
// - Y : 128
// - X_SCALE : 0.83
// - Y_SCALE : 0.25
// - ROT_ANGLE : 2.3

int main() {
  
  // We read the image from the aux dir. Note that we will use a cv:Mat type,
  // so the ModelFitting/Image/OpenCvMatImageTraits.h must be included.
  cv::Mat image;
  double pixel_scale {};
  auto image_path = Elements::pathSearchInEnvVariable("gal.fits", "ELEMENTS_AUX_PATH");
  tie(image, pixel_scale) = readImage(image_path[0].string());
  
  //
  // Model creation
  //
  // The frame model we will use will contain a single extended model, with a
  // single exponential component.
  
  // First we define the parameters of the exponential. We are going to minimize
  // only the I0, so it is the only EngineParameter. For the engine parameters
  // we need to use a coordinate converter. The options are:
  // - NeutralConverter : Does no conversion
  // - NormalizedConverter : Normalizes the parameter so the engine value is 1
  //                         for a specific world value
  // - SigmoidConverter : Converts the parameter using the sigmoid function
  // - ExpSigmoidConverter : Converts the parameter using the exponential sigmoid function
  EngineParameter i0 {50000., make_unique<ExpSigmoidConverter>(1, 1000000.)};
  ManualParameter n {1.};
  ManualParameter k {1.};
  
  // We create the component list of the extended model with the single exponential
  auto reg_man = make_unique<OnlySmooth>();
  auto exp = make_unique<SersicModelComponent>(move(reg_man), i0, n, k);
  vector<unique_ptr<ModelComponent>> component_list {};
  component_list.emplace_back(move(exp));
  
  // We create the extended model. All of its parameters will be optimized by
  // the minimization engine.
  EngineParameter x {128., make_unique<NormalizedConverter>(150.)};
  EngineParameter y {128., make_unique<NormalizedConverter>(150.)};
  EngineParameter x_scale {4.0, make_unique<SigmoidConverter>(0, 100.)};
  EngineParameter y_scale {1.0, make_unique<SigmoidConverter>(0, 100.)};
  EngineParameter rot_angle {20.0 * M_PI/180.0, make_unique<SigmoidConverter>(0, 2*M_PI)};
  
  // The size of the extended model (??? from the detection step ???)
  double width = 128;
  double height = 128;
  
  // We create the extended model list with a single model
  vector<ExtendedModel> extended_models {};
  extended_models.emplace_back(std::move(component_list), x_scale, y_scale,
                               rot_angle, width, height, x, y);
  //  ExtendedModel extended_model {move(component_list),x_scale, y_scale,
  //                                rot_angle, width, height, x, y};
  auto image2 = extended_models[0].getRasterizedImage<cv::Mat>(.2, 257, 257);
  writeToFits(image2, "example3b.fits");

  // We read the PSF from the file
  auto psf_path = Elements::pathSearchInEnvVariable("psf_gal.fits", "ELEMENTS_AUX_PATH");
  auto psf = readPsf(psf_path[0].string());
  
  // Finally we create the FrameModel with same pixel scale and size as the
  // input image
  FrameModel<OpenCvPsf, cv::Mat> frame_model {
    pixel_scale, (size_t)image.cols, (size_t)image.rows, {}, {},
    move(extended_models), move(psf)
  };
  
  //
  // Minimization
  //
  
  // First we need to specify which parameters are optimized by the engine
  EngineParameterManager manager {};
  manager.registerParameter(i0);
  manager.registerParameter(x);
  manager.registerParameter(y);
  manager.registerParameter(x_scale);
  manager.registerParameter(y_scale);
  manager.registerParameter(rot_angle);
  
  // Now we need to create the DataVsModelResiduals. We will set all the weights
  // as ones and we will use the LogChiSquareComparator.
  // Note that because we use cv::Mat as input we have to include the file
  // ModelFitting/Engine/OpenCvDataVsModelInputTraits.h
  cv::Mat weight = cv::Mat::ones(image.rows, image.cols, CV_64F);
  auto data_vs_model = createDataVsModelResiduals(std::move(image), std::move(frame_model),
                                                  std::move(weight), LogChiSquareComparator{});
                                                  
  // We create a residual estimator and we add our block provider
  ResidualEstimator res_estimator {};
  res_estimator.registerBlockProvider(move(data_vs_model));
  
  // We print the parameters before the minimization for comparison
  cout << "I0 (12) = " << i0.getValue() << '\n';
  cout << "X (14.5) = " << x.getValue() << '\n';
  cout << "Y (15.3) = " << y.getValue() << '\n';
  cout << "X_SCALE (.83) = " << x_scale.getValue() << '\n';
  cout << "Y_SCALE (.25) = " << y_scale.getValue() << '\n';
  cout << "angle (2.3) = " << rot_angle.getValue() << '\n';
  
  // Finally we create a levmar engine and we solve the problem
  LevmarEngine engine {};
  auto t1 = chrono::steady_clock::now();
  auto solution = engine.solveProblem(manager, res_estimator);
  auto t2 = chrono::steady_clock::now();
  
  // We print the results
  cout << "\nTime of fitting: " << chrono::duration <double, milli> (t2-t1).count() << " ms" << endl;
  cout << "\n";
  
  cout << "I0 (12) = " << i0.getValue() << '\n';
  cout << "X (14.5) = " << x.getValue() << '\n';
  cout << "Y (15.3) = " << y.getValue() << '\n';
  cout << "X_SCALE (.83) = " << x_scale.getValue() << '\n';
  cout << "Y_SCALE (.25) = " << y_scale.getValue() << '\n';
  cout << "angle (2.3) = " << rot_angle.getValue() << '\n';
 
  printLevmarInfo(boost::any_cast<array<double,10>>(solution.underlying_framework_info));

  // We create the component list of the extended model with the single exponential
  reg_man = make_unique<OnlySmooth>();
  exp = make_unique<SersicModelComponent>(move(reg_man), i0, n, k);
  vector<unique_ptr<ModelComponent>> component_list2 {};
  component_list2.emplace_back(move(exp));
  ExtendedModel extended_model {move(component_list2),x_scale, y_scale,
                                rot_angle, width, height, x, y};
  image2 = extended_model.getRasterizedImage<cv::Mat>(.2, 257, 257);
  writeToFits(image2, "example3b2.fits");

}
