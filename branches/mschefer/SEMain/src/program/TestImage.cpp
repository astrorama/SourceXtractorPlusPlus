/*
 * TestImage.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: mschefer
 */

#include <iostream>
#include <tuple>
#include <vector>
#include <valarray>

#include <cstdlib>
#include <ctime>

#include <boost/any.hpp>
#include <boost/random.hpp>

#include <CCfits/CCfits>

#include "SEImplementation/Image/ImageInterfaceTraits.h"

#include "ModelFitting/utils.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/OldSharp.h"
#include "ModelFitting/Models/AutoSharp.h"
#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FrameModel.h"

#include "ModelFitting/Image/OpenCvMatImageTraits.h"
#include "ModelFitting/Image/OpenCvPsf.h"
#include "ModelFitting/Engine/OpenCvDataVsModelInputTraits.h"

#include "SEImplementation/Configuration/PsfConfig.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"

using namespace SExtractor;
using namespace ModelFitting;

const double pixel_scale = 1.0;
const unsigned int image_size = 512;

const int point_sources_nb = 5;
const double point_min_i0 = 100000;
const double point_max_i0 = 300000;

const int gal_sources_nb = 0;
const double gal_exp_min_i0 = 100000;
const double gal_exp_max_i0 = 100001;
const double gal_dev_min_i0 = 100000;
const double gal_dev_max_i0 = 100001;

const double background_level = 0.0;
const double background_sigma = 20.0;

/// Writes an OpenCv Mat to an image FITS file (prepend the filename with '!' to
/// override existing files)
void writeToFits(const cv::Mat& image, const std::string& filename) {
  std::valarray<double> data (image.total());
  std::copy(image.begin<double>(), image.end<double>(), begin(data));
  long naxis = 2;
  long naxes[2] = {image.size[1], image.size[0]};
  std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS("!"+filename, DOUBLE_IMG, naxis, naxes)};
  pFits->pHDU().write(1, image.total(), data);
  std::cout << "Created file " << filename << '\n';
}

void addExtendedSource(std::vector<ExtendedModel>& extended_models,
    double x, double y, double exp_flux, double exp_rad, double exp_aspect, double exp_rot, double dev_flux, double dev_rad) {

  ManualParameter x_param {x};
  ManualParameter y_param {y};

  {
    // Exponential component

    // Parameters
    double size = image_size;
    ManualParameter xs (1);
    ManualParameter ys (exp_aspect);
    ManualParameter rot (exp_rot);
    ManualParameter exp_n { 1 };

    ManualParameter exp_k { 1.7 / exp_rad };
    ManualParameter exp_i0 { exp_flux / (M_PI * 2.0 * 0.346 * exp_rad * exp_rad) };

    // Model
    std::vector<std::unique_ptr<ModelComponent>> component_list {};
    auto exp = make_unique<SersicModelComponent>(make_unique<OnlySmooth>(), exp_i0, exp_n, exp_k);
    component_list.clear();
    component_list.emplace_back(std::move(exp));
    extended_models.emplace_back(std::move(component_list), xs, ys, rot, size, size, x_param, y_param);
  }
//  {
//    // Devaucouleurs component
//    double size = image_size;
//    ManualParameter xs (1);
//    ManualParameter ys (exp_aspect);
//    ManualParameter rot (exp_rot);
//    ManualParameter dev_n { 4 };
//
//    ManualParameter dev_k { pow(3459.0 / dev_rad, .25) };
//    ManualParameter dev_i0 { dev_flux * pow(10, 3.33) / (7.2 * M_PI * dev_rad * dev_rad) };
//
//    std::vector<std::unique_ptr<ModelComponent>> component_list {};
//    auto exp = make_unique<SersicModelComponent>(make_unique<AutoSharp>(), dev_i0, dev_n, dev_k);
//    component_list.clear();
//    component_list.emplace_back(std::move(exp));
//    extended_models.emplace_back(std::move(component_list), xs, ys, rot, size, size, x_param, y_param);
//  }

}



int main() {
  boost::random::mt19937 rng(time(NULL));

  std::vector<ConstantModel> constant_models;
  std::vector<ExtendedModel> extended_models;
  std::vector<PointModel> point_models;

  //auto psf = PsfConfig::generateGaussianPsf(5.0, .2);
  auto psf = PsfConfig::readPsf("sim01_g_01.psf");


  //addExtendedSource(extended_models, image_size / 2.0, image_size / 2.0, 150000, 2.0, 1, 0,  100000, .1);

  boost::random::uniform_real_distribution<> random_x(image_size *.1, image_size * .9);
  boost::random::uniform_real_distribution<> random_y(image_size *.1, image_size * .9);

  boost::random::uniform_real_distribution<> random_i0(point_min_i0, point_max_i0);
  for (int i = 0; i<point_sources_nb; i++) {
    ManualParameter x (random_x(rng));
    ManualParameter y (random_y(rng));
    ManualParameter i0 (random_i0(rng));
    point_models.emplace_back(x,y,i0);
  }

  boost::random::uniform_real_distribution<> gal_exp_random_i0(gal_exp_min_i0, gal_exp_max_i0);
  boost::random::uniform_real_distribution<> gal_dev_random_i0(gal_dev_min_i0, gal_dev_max_i0);
  for (int i = 0; i<gal_sources_nb; i++) {
    auto total_flux = gal_exp_random_i0(rng);
    addExtendedSource(extended_models, random_x(rng), random_y(rng),
        gal_exp_random_i0(rng),
        5, 1, 0,
//        boost::random::uniform_real_distribution<>(.5, 6)(rng),
//        boost::random::uniform_real_distribution<>(.8, 1)(rng),
//        boost::random::uniform_real_distribution<>(-M_PI/2, M_PI)(rng),
        gal_dev_random_i0(rng),
        boost::random::uniform_real_distribution<>(.1, .5)(rng)
        );
  }


  FrameModel<OpenCvPsf, cv::Mat> frame_model {
    pixel_scale,
    image_size, image_size,
    std::move(constant_models),
    std::move(point_models),
    std::move(extended_models),
    *psf
  };

  auto image = frame_model.getImage();


  // Add noise
  boost::random::normal_distribution<> bg_noise_dist(background_level, background_sigma);
  for (int y=0; y < image.rows; y++) {
    for (int x=0; x < image.cols; x++) {

//      // add Poisson noise
//      if (image.at<double>(y, x) > 0.) {
//        image.at<double>(y, x) = boost::random::poisson_distribution<>(image.at<double>(y, x))(rng);
//      }
      // background (gaussian) noise
      image.at<double>(y, x) += bg_noise_dist(rng);
    }
  }


  writeToFits(image, "test_image.fits");

  std::cout << "All done ^__^\n";

  return 0;
}


