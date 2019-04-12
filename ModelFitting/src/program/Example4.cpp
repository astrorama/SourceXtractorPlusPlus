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
/** 
 * @file Examole4.cpp
 * @date September 3, 2015
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

int main(int argc, char **argv) {
  std::string engine_impl("levmar");
  if (argc > 1) {
    engine_impl = argv[1];
  }

  auto frames_path = Elements::pathSearchInEnvVariable("multiframe.fits", "ELEMENTS_AUX_PATH");
  auto frames = readFrames(frames_path[0].string());
  
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
  EngineParameter i0 {12., make_unique<ExpSigmoidConverter>(1, 100)};
  ManualParameter n {1.};
  ManualParameter k {1.};
  
  // We create the component list of the extended model with the single exponential
  auto reg_man = make_unique<OnlySmooth>();
  auto exp = make_unique<SersicModelComponent>(move(reg_man), i0, n, k);
  vector<unique_ptr<ModelComponent>> component_list {};
  component_list.emplace_back(move(exp));
  
  // We create the extended model. All of its parameters will be optimized by
  // the minimization engine.
  EngineParameter x {10, make_unique<NormalizedConverter>(30.)};
  EngineParameter y {20, make_unique<NormalizedConverter>(30.)};
  EngineParameter x_scale {.5, make_unique<SigmoidConverter>(0, 1)};
  EngineParameter y_scale {.5, make_unique<SigmoidConverter>(0, 1)};
  EngineParameter rot_angle {2., make_unique<SigmoidConverter>(0, 2*M_PI)};
  
  // The size of the extended model (??? from the detection step ???)
  double width = 10;
  double height = 10;
  
  // We create the extended model list with a single model
  vector<TransformedModel> extended_models {};
  extended_models.emplace_back(std::move(component_list), x_scale, y_scale,
                               rot_angle, width, height, x, y);
  
  // We read the PSF from the file
  auto psf_path = Elements::pathSearchInEnvVariable("psf.fits", "ELEMENTS_AUX_PATH");
  auto psf = readPsf(psf_path[0].string());
  
  ResidualEstimator res_estimator {};
  
  for (auto& pair : frames) {
    cv::Mat image;
    double pixel_scale {};
    tie(image, pixel_scale) = pair;
    
    FrameModel<OpenCvPsf, cv::Mat> frame_model {
      pixel_scale, (size_t)image.cols, (size_t)image.rows, {}, {},
      move(extended_models), move(psf)
    };
    
    cv::Mat weight = cv::Mat::ones(image.rows, image.cols, CV_64F);
    auto data_vs_model = createDataVsModelResiduals(std::move(image), std::move(frame_model),
                                                    std::move(weight), LogChiSquareComparator{});
    res_estimator.registerBlockProvider(move(data_vs_model));
  }
  
  //
  // Minimization
  //
  
  // We print the parameters before the minimization for comparison
  cout << "I0 (12) = " << i0.getValue() << '\n';
  cout << "X (14.5) = " << x.getValue() << '\n';
  cout << "Y (15.3) = " << y.getValue() << '\n';
  cout << "X_SCALE (.83) = " << x_scale.getValue() << '\n';
  cout << "Y_SCALE (.25) = " << y_scale.getValue() << '\n';
  cout << "angle (2.3) = " << rot_angle.getValue() << '\n';
  
  // First we need to specify which parameters are optimized by the engine
  EngineParameterManager manager {};
  manager.registerParameter(i0);
  manager.registerParameter(x);
  manager.registerParameter(y);
  manager.registerParameter(x_scale);
  manager.registerParameter(y_scale);
  manager.registerParameter(rot_angle);
  
  // Finally we create a least square engine and we solve the problem
  std::cout << "Registered engines: " << std::endl;
  for (auto &e : LeastSquareEngine::getImplementations()) {
    std::cout << '\t' << e << std::endl;
  }
  std::cout << "Using engine " << engine_impl << std::endl;
  auto engine = LeastSquareEngine::create(engine_impl);
  auto t1 = chrono::steady_clock::now();
  auto solution = engine->solveProblem(manager, res_estimator);
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
  
}
