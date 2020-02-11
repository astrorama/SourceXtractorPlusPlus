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
 * @file Example1.cpp
 * @date September 2, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/AutoSharp.h"
#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Image/OpenCvMatImageTraits.h"
#include "utils.h"

using namespace std;
using namespace ModelFitting;
using Euclid::make_unique;

//
// This example demonstrates the following:
//
// - How to create parameters
// - How to create extended model components from a Profile
// - How to use decorators for scaling and rotating a model component
// - How to create an ExtendedModel from its model components
//

int main() {
  
  //
  // Creation of the first model component (exponential profile)
  //
  
  // First we need to create the parameters the profile will use. We need the
  // three parameters of the Sersic profile.
  auto exp_i0 = std::make_shared<ManualParameter>(12.6);
  auto exp_n = std::make_shared<ManualParameter>(1.);
  auto exp_k = std::make_shared<ManualParameter>(1.);
  
  // We want to create a CircularlySymmetricModelComponent. For this reason we
  // need a SharpRegionManager, which describes how the sharp region is computed.
  // There are three different options:
  // - OnlySmooth : The profile is treated as there is no sharp region.
  // -   OldSharp : The behavior of the SExtractor2. The profile MUST be zero
  //                outside the sharp radius.
  // -  AutoSharp : The sharp region is automatically calculated.
  //
  // For the exponential profile we know that it is smooth enough, so we use
  // the OnlySmooth SharpRegionManager. This is increasing the performance.
  auto exp_reg_man = make_unique<OnlySmooth>();
  
  // Here we create the CircularlySymmetricModelComponent. The profile type is
  // a template parameter of the class and we want to use a SersicProfile. That
  // means we should use the type CircularlySymmetricModelComponent<SersicProfile>.
  // For simplicity, an alias of this type is pre-defined with the name
  // SersicModelComponent.
  auto exp = make_unique<SersicModelComponent>(move(exp_reg_man), exp_i0, exp_n, exp_k);
  
  // We use the ScaledModelComponent decorator for applying the axes scaling.
  // For this we need two more parameters describing the scaling factors.
  auto x_scale = std::make_shared<ManualParameter>(2.);
  auto y_scale = std::make_shared<ManualParameter>(.5);
  auto scaled_exp = make_unique<ScaledModelComponent>(move(exp), x_scale, y_scale);
  
  // Similarly we use the RotatedModelComponent decorator to rotate the already
  // scaled component by 30 degrees
  auto exp_rot_angle = std::make_shared<ManualParameter>(M_PI / 6.);
  auto rotated_exp = make_unique<RotatedModelComponent>(move(scaled_exp), exp_rot_angle);
  
  //
  // Creation of the second model component (De Vaucouleurs profile)
  //
  
  // We perform the same steps with above to create the second model component,
  // with the following differences:
  // - We use the De Vaucouleurs profile instead of the exponential
  // - We use the AutoSharp SharpRegionManager because the center of the profile
  //   is too sharp
  // - We rotate it by 30 degrees to the opposite direction
  // Note that because we use the same scaling factors we reuse the same parameters.
  auto dev_i0 = std::make_shared<ManualParameter>(525.3);
  auto dev_n = std::make_shared<ManualParameter>(4.);
  auto dev_k = std::make_shared<ManualParameter>(7.66924944);
  auto dev_reg_man = make_unique<AutoSharp>();
  auto dev = make_unique<SersicModelComponent>(move(dev_reg_man), dev_i0, dev_n, dev_k);
  auto scaled_dev = make_unique<ScaledModelComponent>(move(dev), x_scale, y_scale);
  auto dev_rot_angle = std::make_shared<ManualParameter>(-M_PI / 6.);
  auto rotated_dev = make_unique<RotatedModelComponent>(move(scaled_dev), dev_rot_angle);
  
  //
  // Creation of the extended model
  //
  
  // First we need to create the parameters required by the extended model. These
  // are the following:
  // - The x and y position (in pixels) at the frame. We do not care about their
  //   values in this example.
  // - The x and y scale of the extended model. It is applied to all the components.
  //   We do not apply any scaling at this example.
  // - The rotation of the extended model. It is applied to all the components.
  // - The size (in arcsec) of the model (???from the detection step???)
  auto x = std::make_shared<ManualParameter>(0);
  auto y = std::make_shared<ManualParameter>(0);
  auto model_scale = std::make_shared<ManualParameter>(1.);
  auto model_angle = std::make_shared<ManualParameter>(M_PI / 4.);
  double width = 10.;
  double height = 10.;
  
  // To create the extended model we first create a vector with its model components
  vector<unique_ptr<ModelComponent>> component_list {};
  component_list.emplace_back(move(rotated_exp));
  component_list.emplace_back(move(rotated_dev));
  
  // We finally create the extended model
  ExtendedModel extended_model {move(component_list), model_scale, model_scale,
                                model_angle, width, height, x, y};
                                
  //
  // Model demonstration
  //
  
  // As an example of using the extended model we are using its method for computing
  // its rasterized image. As parameters we pass the pixel scale, the width and
  // the height of the image we want. Note that the dimensions have to be odd
  // numbers and that the model is always centered at the center of the image.
  // 
  // The template parameter controls the type of the image the method returns.
  // It can be any type for which a specialization of the ImageTraits templated
  // class is provided. For the cv::Mat this specialization is defined in the
  // ModelFitting/Image/OpenCvMatImageTraits.h file, which is included at the
  // top of this file.
  auto image = extended_model.getRasterizedImage<cv::Mat>(.1, 201, 301);
  writeToFits(image, "example1.fits");
  
}
