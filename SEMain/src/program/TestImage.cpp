/*
 * TestImage.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: mschefer
 */

#include "ElementsKernel/ProgramHeaders.h"


#include <iostream>
#include <tuple>
#include <vector>
#include <valarray>

#include <cstdlib>
#include <ctime>

#include <boost/any.hpp>
#include <boost/random.hpp>

#include <CCfits/CCfits>

#include "SEFramework/Image/SubtractImage.h"


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
#include "SEImplementation/Image/ImagePsf.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace SExtractor;
using namespace ModelFitting;

const double pixel_scale = 1.0;

const int point_sources_nb = 5;
const double point_min_i0 = 100000;
const double point_max_i0 = 300000;

const int gal_sources_nb = 5;
const double gal_exp_min_i0 = 100000;
const double gal_exp_max_i0 = 100001;
const double gal_dev_min_i0 = 100000;
const double gal_dev_max_i0 = 100001;

struct TestImageSource {
  double x, y;
  double exp_flux, exp_rad, exp_aspect, exp_rot;
  double dev_flux, dev_rad, dev_aspect, dev_rot;
};

class TestImage : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description config_options { "TestImage options" };

    // Add the specific program options
    config_options.add_options()
        ("output", po::value<string>()->required(), "filename to save the created test image")
        ("size", po::value<double>()->default_value(512.0), "image size")
        ("bg-sigma", po::value<double>()->default_value(20.0), "standard deviation of background gaussian noise")
        ("gain", po::value<double>()->default_value(0.0), "gain in e-/adu, 0 for infinite gain")
        ("saturation", po::value<double>()->default_value(0.0), "image saturation level, 0 for no saturation")
        ("psf-file", po::value<string>()->default_value(""), "Psf file for convolution")
        ("psf-fwhm", po::value<double>()->default_value(5.0),
            "Full width half maximum for generated gaussian psf (used when no psf file is provided)")
        ("psf-scale", po::value<double>()->default_value(0.2),
            "Pixel scale for generated gaussian psf")
        ("random-sources", po::value<int>()->default_value(0),
            "Nb of random sources to add")
        ;

    return config_options;
  }


  /// Writes a VectorImage to an image FITS file (prepend the filename with '!' to
  /// override existing files)
  void writeToFits(std::shared_ptr<VectorImage<SeFloat>> image, const std::string& filename) {
    std::valarray<double> data (image->getWidth() * image->getHeight());
    std::copy(image->getData().begin(), image->getData().end(), begin(data));
    long naxis = 2;
    long naxes[2] = {image->getWidth(), image->getHeight()};
    std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS("!"+filename, DOUBLE_IMG, naxis, naxes)};
    pFits->pHDU().write(1, data.size(), data);
  }

  void addExtendedSource(std::vector<ExtendedModel>& extended_models, double size, const TestImageSource& source) {

    ManualParameter x_param {source.x};
    ManualParameter y_param {source.y};

    {
      // Exponential component

      // Parameters
      ManualParameter xs (1);
      ManualParameter ys (source.exp_aspect);
      ManualParameter rot (source.exp_rot);
      ManualParameter exp_n { 1 };

      ManualParameter exp_k { 1.7 / source.exp_rad };
      ManualParameter exp_i0 { source.exp_flux / (M_PI * 2.0 * 0.346 * source.exp_rad * source.exp_rad) };

      // Model
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto exp = make_unique<SersicModelComponent>(make_unique<AutoSharp>(), exp_i0, exp_n, exp_k);
      component_list.clear();
      component_list.emplace_back(std::move(exp));
      extended_models.emplace_back(std::move(component_list), xs, ys, rot, size, size, x_param, y_param);
    }
    {
      // Devaucouleurs component
      ManualParameter xs (1);
      ManualParameter ys (source.dev_aspect);
      ManualParameter rot (source.dev_rot);
      ManualParameter dev_n { 4 };

      ManualParameter dev_k { pow(3459.0 / source.dev_rad, .25) };
      ManualParameter dev_i0 { source.dev_flux * pow(10, 3.33) / (7.2 * M_PI * source.dev_rad * source.dev_rad) };

      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto exp = make_unique<SersicModelComponent>(make_unique<AutoSharp>(), dev_i0, dev_n, dev_k);
      component_list.clear();
      component_list.emplace_back(std::move(exp));
      extended_models.emplace_back(std::move(component_list), xs, ys, rot, size, size, x_param, y_param);
    }
  }

  void addPointSource(std::vector<PointModel>& point_models, double x, double y, double flux) {
    ManualParameter x_param (x);
    ManualParameter y_param (y);
    ManualParameter flux_param (flux);

    point_models.emplace_back(x_param, y_param, flux_param);
  }

  void addBackgroundNoise(std::shared_ptr<VectorImage<SeFloat>> image, double background_level, double background_sigma) {
    // Add noise
    boost::random::normal_distribution<> bg_noise_dist(background_level, background_sigma);
    for (int y=0; y < image->getHeight(); y++) {
      for (int x=0; x < image->getWidth(); x++) {
        // background (gaussian) noise
        image->at(x, y) += bg_noise_dist(rng);
      }
    }
  }

  void addPoissonNoise(std::shared_ptr<VectorImage<SeFloat>> image, double gain) {
    // Add noise
    if (gain > 0.0) {
      for (int y=0; y < image->getHeight(); y++) {
        for (int x=0; x < image->getWidth(); x++) {
          if (image->at(x, y) > 0.) {
            image->at(x, y) = boost::random::poisson_distribution<>(image->at(x, y) * gain)(rng) / gain;
          }
        }
      }
    }
  }

  void saturate(std::shared_ptr<VectorImage<SeFloat>> image, double saturation_level) {
    if (saturation_level > 0.0) {
      for (int y=0; y < image->getHeight(); y++) {
        for (int x=0; x < image->getWidth(); x++) {
          image->at(x, y) = std::min(image->at(x, y), (float) saturation_level);
        }
      }
    }
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    Elements::Logging logger = Elements::Logging::getLogger("TestImage");

    auto image_size = args["size"].as<double>();

    std::vector<ConstantModel> constant_models;
    std::vector<ExtendedModel> extended_models;
    std::vector<PointModel> point_models;

    std::shared_ptr<ImagePsf> psf;
    auto psf_filename = args["psf-file"].as<std::string>();
    if (psf_filename != "") {
      logger.info() << "Loading psf file: " << psf_filename;
      psf = PsfConfig::readPsf(psf_filename);
    } else {
      psf = PsfConfig::generateGaussianPsf(args["psf-fwhm"].as<double>(), args["psf-scale"].as<double>());
    }

/*
    //addExtendedSource(extended_models, image_size / 2.0, image_size / 2.0, 150000, 2.0, 1, 0,  100000, .1);
//    boost::random::uniform_real_distribution<> random_i0(point_min_i0, point_max_i0);
//    for (int i = 0; i<point_sources_nb; i++) {
//      ManualParameter x (random_x(rng));
//      ManualParameter y (random_y(rng));
//      ManualParameter i0 (random_i0(rng));
//      point_models.emplace_back(x,y,i0);
//    }


    std::vector<TestImageSource> sources;

    logger.info("Adding random sources...");

    boost::random::uniform_real_distribution<> random_x(image_size *.1, image_size * .9);
    boost::random::uniform_real_distribution<> random_y(image_size *.1, image_size * .9);
    boost::random::uniform_real_distribution<> gal_exp_random_i0(gal_exp_min_i0, gal_exp_max_i0);
    boost::random::uniform_real_distribution<> gal_dev_random_i0(gal_dev_min_i0, gal_dev_max_i0);

    for (int i = 0; i< args["random-sources"].as<int>(); i++) {
      //auto total_flux = gal_exp_random_i0(rng);
      sources.push_back( {random_x(rng), random_y(rng),
          gal_exp_random_i0(rng),
          boost::random::uniform_real_distribution<>(.5, 6)(rng),
          boost::random::uniform_real_distribution<>(.8, 1)(rng),
          boost::random::uniform_real_distribution<>(-M_PI/2, M_PI/2)(rng),

          gal_dev_random_i0(rng),
          boost::random::uniform_real_distribution<>(.1, .5)(rng),
          boost::random::uniform_real_distribution<>(.8, 1)(rng),
          boost::random::uniform_real_distribution<>(-M_PI/2, M_PI/2)(rng) }
          );
    }

    logger.info("Creating source models...");
    for (const auto& source : sources) {
      addExtendedSource(extended_models, image_size, source);
    }

    logger.info("Rendering...");

    FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model {
      pixel_scale,
      image_size, image_size,
      std::move(constant_models),
      std::move(point_models),
      std::move(extended_models),
      *psf
    };

    auto image = frame_model.getImage();

    logger.info("Adding noise...");

    addPoissonNoise(image, args["gain"].as<double>());
    addBackgroundNoise(image, 0, args["bg-sigma"].as<double>());
    saturate(image, args["saturation"].as<double>());

    auto filename = args["output"].as<std::string>();
    logger.info() << "Saving file: " << filename;
    writeToFits(image, filename);
    */

    Test(image_size, psf, args["output"].as<std::string>());

    logger.info("All done ^__^");
    return Elements::ExitCode::OK;
  }

  void Test(double image_size, std::shared_ptr<ImagePsf> psf, std::string filename) {
    Elements::Logging logger = Elements::Logging::getLogger("Test!!!");

    std::vector<ConstantModel> constant_models;
    std::vector<ExtendedModel> extended_models;
    std::vector<PointModel> point_models;

    std::vector<TestImageSource> sources;

    sources.push_back({
       image_size / 2.0, image_size / 2.0, //double x, y;
       10000, .25, 1, 0,//double exp_flux, exp_rad, exp_aspect, exp_rot;
       0, 1, 1, 0//double dev_flux, dev_rad, dev_aspect, dev_rot
    });


    logger.info("Creating source models...");
    for (const auto& source : sources) {
      addExtendedSource(extended_models, image_size, source);
    }

    addPointSource(point_models, image_size / 2.0 + 100, image_size / 2.0, 10000);


    logger.info("Rendering...");

    FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> frame_model {
      pixel_scale,
      image_size, image_size,
      std::move(constant_models),
      std::move(point_models),
      std::move(extended_models),
      *psf
    };

    auto image = frame_model.getImage();

    //addPoissonNoise(image, 1);
    addBackgroundNoise(image, 0, 1);

    logger.info() << "Saving file: " << filename;
    writeToFits(image, filename);
    //writeToFits(VectorImage<SeFloat>::create(*SubtractImage<SeFloat>::create(image_1, image_2)), filename);
  }

private:
  boost::random::mt19937 rng { time(NULL) } ;

};

MAIN_FOR(TestImage)

