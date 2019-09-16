/*
 * TestImage.cpp
 *
 *  Created on: Jul 12, 2017
 *      Author: mschefer
 */

#include "ElementsKernel/ProgramHeaders.h"


#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <valarray>
#include <numeric>

#include <cstdlib>
#include <ctime>

#include <boost/any.hpp>
#include <boost/random.hpp>

#include <CCfits/CCfits>
#include "SEFramework/Image/SubtractImage.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEImplementation/Image/WriteableImageInterfaceTraits.h"
#include "SEFramework/Image/FitsImageSource.h"
#include "SEFramework/Image/WriteableBufferedImage.h"
#include "SEImplementation/Plugin/Psf/PsfPluginConfig.h"
#include "SEImplementation/Image/ImagePsf.h"

#include "SEImplementation/CoordinateSystem/WCS.h"

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
#include "ModelFitting/Image/NullPsf.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace SExtractor;
using namespace ModelFitting;

const double pixel_scale = 1.0;

const double gal_exp_min_i0 = 100000;
const double gal_exp_max_i0 = 100001;
const double gal_dev_min_i0 = 100000;
const double gal_dev_max_i0 = 100001;

struct TestImageSource {
  double x, y;
  double exp_flux, exp_rad, exp_aspect, exp_rot;
  double dev_flux, dev_rad, dev_aspect, dev_rot;
  double point_flux;
};

//
class DummyWCS : public CoordinateSystem {
public:
  DummyWCS(int image_width, int image_height, double rotation, double scale, double shift_x, double shift_y)
      : m_image_width(image_width), m_image_height(image_height),
        m_rotation(rotation), m_scale(1.0/scale), m_shift_x(shift_x), m_shift_y(shift_y) {}
  virtual ~DummyWCS() {}

  virtual WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const {
    return WorldCoordinate(image_coordinate.m_x, image_coordinate.m_y);
  }

  virtual ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const {
    return ImageCoordinate(world_coordinate.m_alpha, world_coordinate.m_delta);
  }

  virtual std::map<std::string, std::string> getFitsHeaders() const override {
    auto c = cos(m_rotation);
    auto s = sin(m_rotation);

    return {
      {"CTYPE1", "'RA---TAN'"},
      {"CTYPE2", "'DEC--TAN'"},
      {"CUNIT1", "'deg'"},
      {"CUNIT2", "'deg'"},
      {"RADSYS", "'ICRS'"},
      {"WCSAXES", "2"},
      {"LATPOLE", std::to_string(10.0)},
      {"LONPOLE", std::to_string(180.0)},
      {"CDELT1", std::to_string(1.0)},
      {"CDELT2", std::to_string(1.0)},
      {"CRVAL1", std::to_string(10.0)},
      {"CRVAL2", std::to_string(10.0)},
      {"CRPIX1", std::to_string(m_image_width / 2.0 + 1.5 + m_shift_x)},
      {"CRPIX2", std::to_string(m_image_height / 2.0 + 1.5 + m_shift_y)},

      {"PC1_1", std::to_string(0.001 * c * m_scale)},
      {"PC1_2", std::to_string(0.001 * s * m_scale)},
      {"PC2_1", std::to_string(0.001 * -s * m_scale)},
      {"PC2_2", std::to_string(0.001 * c * m_scale)}
    };
  }

private:
  int m_image_width, m_image_height;
  double m_rotation = 0.0;
  double m_scale = 1.0;
  double m_shift_x = 0.0;
  double m_shift_y = 0.0;
};

template <typename ImageType>
class DummyPsf {
public:
  DummyPsf() : m_kernel(VectorImage<SeFloat>::create(1, 1)) {}

  double getPixelScale() const {
    return 1.0;
  }

  std::size_t getSize() const {
    return 1;
  }

  std::shared_ptr<VectorImage<SExtractor::SeFloat>>  getScaledKernel(double /*scale*/) const {
    return m_kernel;
  }

  void convolve(ImageType& /*image*/) const {
  }

private:
  std::shared_ptr<VectorImage<SExtractor::SeFloat>>  m_kernel;

};

class TestImage : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description config_options { "TestImage options" };

    // Add the specific program options
    config_options.add_options()
        ("output", po::value<string>()->required(), "filename to save the created test image")
        ("output-weight", po::value<string>()->default_value(""), "filename to save the created weight map image")
        ("size", po::value<double>()->default_value(512.0), "image size")
        ("bg-level", po::value<double>()->default_value(0.0), "background level")
        ("bg-sigma", po::value<double>()->default_value(20.0), "standard deviation of background gaussian noise")
        ("gain", po::value<double>()->default_value(0.0), "gain in e-/adu, 0 for infinite gain")
        ("saturation", po::value<double>()->default_value(0.0), "image saturation level, 0 for no saturation")
        ("psf-file", po::value<string>()->default_value(""), "Psf file for convolution")
        ("psf-fwhm", po::value<double>()->default_value(5.0),
            "Full width half maximum for generated gaussian psf (used when no psf file is provided)")
        ("psf-scale", po::value<double>()->default_value(0.2), "Pixel scale for generated gaussian psf")
        ("disable-psf", po::bool_switch(), "Disable psf convolution")
        ("random-sources", po::value<int>()->default_value(0), "Nb of random sources to add")
        ("source-list", po::value<string>()->default_value(""), "Use sources from file")
        ("source-catalog", po::value<string>()->default_value(""), "Use sources from file (skymaker format)")
        ("zero-point", po::value<double>()->default_value(0.0), "Zero point for magnitudes in catalog")
        ("exposure-time", po::value<double>()->default_value(300.), "Exposure time for objects in catalog")
        ("save-sources", po::value<string>()->default_value(""), "Filename to save final list of sources")
        ("bad-pixels", po::value<double>()->default_value(0.0), "Probability for a pixel to be a bad pixel")
        ("bad-columns", po::value<double>()->default_value(0.0), "Probability for a column of pixels to be bad")
        ("rotation", po::value<double>()->default_value(0.0), "Rotate sources around middle point")
        ("scale", po::value<double>()->default_value(1.0), "Scale factor")
        ("shift-x", po::value<double>()->default_value(0.0), "Shift X")
        ("shift-y", po::value<double>()->default_value(0.0), "Shift Y")
        ("model-size", po::value<double>()->default_value(0.0), "Model size for the rasterization of sources, defaults to size")
        ("max-tile-memory", po::value<int>()->default_value(512), "Maximum memory used for image tiles cache in megabytes")
        ("tile-size", po::value<int>()->default_value(256), "Image tiles size in pixels")
        ("copy-coordinate-system", po::value<string>()->default_value(""), "Copy the coordinate system from another FITS file")
        ;

    return config_options;
  }

  void addSource(std::vector<PointModel>& point_models, std::vector<TransformedModel>& extended_models, double size, const TestImageSource& source, std::tuple<double, double, double, double> jacobian) {

    ManualParameter x_param {source.x};
    ManualParameter y_param {source.y};

    if (source.exp_flux > 0.0) {
      // Exponential component

      // Parameters
      ManualParameter xs (1);
      ManualParameter ys (source.exp_aspect);
      ManualParameter rot (source.exp_rot);
      ManualParameter exp_n { 1 };

      ManualParameter exp_k { 1.7 / source.exp_rad };
      ManualParameter exp_i0 { source.exp_flux / (M_PI * 2.0 * 0.346 * source.exp_rad * source.exp_rad * source.exp_aspect) };

      // Model
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto exp = make_unique<SersicModelComponent>(make_unique<OldSharp>(), exp_i0, exp_n, exp_k);
      component_list.clear();
      component_list.emplace_back(std::move(exp));
      extended_models.emplace_back(std::move(component_list), xs, ys, rot, size, size, x_param, y_param, jacobian);
    }

    if (source.dev_flux > 0.0) {
      // Devaucouleurs component
      ManualParameter xs (1);
      ManualParameter ys (source.dev_aspect);
      ManualParameter rot (source.dev_rot);
      ManualParameter dev_n { 4 };

      ManualParameter dev_k { pow(3459.0 / source.dev_rad, .25) };
      ManualParameter dev_i0 { source.dev_flux * pow(10, 3.33) / (7.2 * M_PI * source.dev_rad * source.dev_rad * source.dev_aspect) };

      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto exp = make_unique<SersicModelComponent>(make_unique<OldSharp>(), dev_i0, dev_n, dev_k);
      component_list.clear();
      component_list.emplace_back(std::move(exp));
      extended_models.emplace_back(std::move(component_list), xs, ys, rot, size, size, x_param, y_param, jacobian);
    }
    if (source.point_flux > 0.0) {
      ManualParameter flux_param (source.point_flux);
      point_models.emplace_back(x_param, y_param, flux_param);
    }
  }

  void addPointSource(std::vector<PointModel>& point_models, double x, double y, double flux) {
    ManualParameter x_param (x);
    ManualParameter y_param (y);
    ManualParameter flux_param (flux);

    point_models.emplace_back(x_param, y_param, flux_param);
  }

  void addBackgroundNoise(std::shared_ptr<WriteableImage<SeFloat>> image, double background_level, double background_sigma) {
    // Add noise
    boost::random::normal_distribution<> bg_noise_dist(background_level, background_sigma);
    for (int y=0; y < image->getHeight(); y++) {
      for (int x=0; x < image->getWidth(); x++) {
        // background (gaussian) noise
        image->setValue(x, y, image->getValue(x, y) + bg_noise_dist(m_rng));
      }
    }
  }

  void addPoissonNoise(std::shared_ptr<WriteableImage<SeFloat>> image, double gain) {
    // Add noise
    if (gain > 0.0) {
      for (int y=0; y < image->getHeight(); y++) {
        for (int x=0; x < image->getWidth(); x++) {
          auto pixel_value = image->getValue(x, y);
          if (pixel_value > 0.) {
            image->setValue(x, y, boost::random::poisson_distribution<>(pixel_value * gain)(m_rng) / gain);
          }
        }
      }
    }
  }

  void saturate(std::shared_ptr<WriteableImage<SeFloat>> image, double saturation_level) {
    if (saturation_level > 0.0) {
      for (int y=0; y < image->getHeight(); y++) {
        for (int x=0; x < image->getWidth(); x++) {
          image->setValue(x, y, std::min(image->getValue(x, y), (float) saturation_level));
        }
      }
    }
  }

  void addBadPixels(std::shared_ptr<WriteableImage<SeFloat>> weight_map, double probability) {
    if (probability>0) {
      for (int y=0; y < weight_map->getHeight(); y++) {
        for (int x=0; x < weight_map->getWidth(); x++) {
          if (boost::random::uniform_01<double>()(m_rng) < probability) {
            weight_map->setValue(x, y, 0);
          }
        }
      }
    }
  }

  void addBadColumns(std::shared_ptr<WriteableImage<SeFloat>> weight_map, double probability) {
    if (probability>0) {
      for (int x=0; x < weight_map->getWidth(); x++) {
        if (boost::random::uniform_01<double>()(m_rng) < probability) {
          for (int y=0; y < weight_map->getHeight(); y++) {
            weight_map->setValue(x, y, 0);
          }
        }
      }
    }
  }

  std::vector<TestImageSource> generateRandomSources(int number_of_sources,
      double x_min, double y_min, double x_max, double y_max) {
    std::vector<TestImageSource> sources;

//    std::cout << x_min << " " << x_max << " " << y_min << " " << y_max << "\n";
//
    boost::random::uniform_real_distribution<> random_x(x_min, x_max);
    boost::random::uniform_real_distribution<> random_y(y_min, y_max);
    boost::random::uniform_real_distribution<> gal_exp_random_i0(gal_exp_min_i0, gal_exp_max_i0);
    boost::random::uniform_real_distribution<> gal_dev_random_i0(gal_dev_min_i0, gal_dev_max_i0);

    for (int i = 0; i < number_of_sources; i++) {
      //auto total_flux = gal_exp_random_i0(rng);
      sources.push_back({
          random_x(m_rng), random_y(m_rng),

          gal_exp_random_i0(m_rng),
          boost::random::uniform_real_distribution<>(.5, 6)(m_rng),
          boost::random::uniform_real_distribution<>(.2, .8)(m_rng),
          boost::random::uniform_real_distribution<>(-M_PI/2, M_PI/2)(m_rng),

          0,0,1,0,
//          gal_dev_random_i0(m_rng),
//          boost::random::uniform_real_distribution<>(.1, .5)(m_rng),
//          boost::random::uniform_real_distribution<>(.8, 1)(m_rng),
//          boost::random::uniform_real_distribution<>(-M_PI/2, M_PI/2)(m_rng),

          0
      });
    }

    return sources;
  }

  std::vector<TestImageSource> loadSourcesFromFile(const std::string& filename) {
    std::vector<TestImageSource> sources;

    std::ifstream file;
    file.open(filename);

    while (file.good()) {
      std::string line;
      std::getline(file, line);
//      line = std::regex_replace(line, std::regex("\\s+#.*"), std::string(""));
//      line = std::regex_replace(line, std::regex("\\s+$"), std::string(""));
      if (line.size() == 0) {
        continue;
      }

      std::stringstream linestream(line);

      TestImageSource source;
      linestream >> source.x >> source.y
          >> source.exp_flux >> source.exp_rad >> source.exp_aspect >> source.exp_rot
          >> source.dev_flux >> source.dev_rad >> source.dev_aspect >> source.dev_rot
          >> source.point_flux;

      source.exp_rot *= -M_PI / 180.0;
      source.dev_rot *= -M_PI / 180.0;

      sources.emplace_back(std::move(source));
    }

    return sources;
  }

  void saveSources(const std::vector<TestImageSource>& sources, const std::string& filename) {
    std::ofstream file;
    file.open(filename);

    for (const auto& source : sources) {
      file << source.x << " " << source.y << " "
          << source.exp_flux << " " << source.exp_rad << " " << source.exp_aspect << " " << (source.exp_rot * -180.0 / M_PI) << " "
          << source.dev_flux << " " << source.dev_rad << " " << source.dev_aspect << " " << (source.dev_rot * -180.0 / M_PI) << " "
          << source.point_flux << "\n";
    }
  }

  void transformSources(std::vector<TestImageSource>& sources, int image_size, double rot_angle, double scale, double shift_x, double shift_y) {
    auto center = image_size / 2.0;
    auto c = cos(rot_angle);
    auto s = sin(rot_angle);
    for (auto& source : sources) {
      source.x -= center;
      source.y -= center;
      double x = (source.x * c - source.y * s) * scale + shift_x;
      double y = (source.x * s + source.y * c) * scale + shift_y;
      source.x = x + center;
      source.y = y + center;

      source.exp_rot -= rot_angle;
      source.dev_rot -= rot_angle;
    }
  }

  std::vector<TestImageSource> loadSourcesFromCatalog(const std::string& filename) {
    std::vector<TestImageSource> sources;

    //std::cout << "^^\n";

    std::ifstream file;
    file.open(filename);

    while (file.good()) {
      std::string line;
      std::getline(file, line);
//      line = std::regex_replace(line, std::regex("\\s+#.*"), std::string(""));
//      line = std::regex_replace(line, std::regex("\\s+$"), std::string(""));
      if (line.size() == 0) {
        continue;
      }

      std::stringstream linestream(line);
      int source_type;
      linestream >> source_type;

      //std::cout << source_type << "\n";

      TestImageSource source;
      if (source_type == 200) {
        double magnitude, bt, exp_angle_degree, dev_angle_degree;

        linestream >> source.x >> source.y >> magnitude
            >> bt
            >> source.dev_rad >> source.dev_aspect >> dev_angle_degree
            >> source.exp_rad >> source.exp_aspect >> exp_angle_degree;

        // FIXME should change sign?
        source.dev_rot = dev_angle_degree * M_PI / 180.0;
        source.exp_rot = exp_angle_degree * M_PI / 180.0;

        auto total_flux = pow(10, (magnitude - m_zero_point) / -2.5);
        source.exp_flux = total_flux * (1.0 - bt);
        source.dev_flux = total_flux * bt;

//        source.dev_rad *= 2;
//        source.exp_rad *= 2;

        source.point_flux = 0;
      } else if (source_type == 100) {
        double magnitude;
        linestream >> source.x >> source.y >> magnitude;
        source.point_flux = pow(10, (magnitude - m_zero_point) / -2.5) * m_exp_time;

        source.exp_flux = source.exp_rot = source.dev_flux = source.dev_rot = 0;
        source.exp_aspect = source.exp_rad = source.dev_aspect = source.dev_rad = 1;
      }

      source.x -= 1.5;
      source.y -= 1.5;

      sources.emplace_back(std::move(source));
    }

    return sources;
  }


  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    Elements::Logging logger = Elements::Logging::getLogger("TestImage");

    auto max_tile_memory = args["max-tile-memory"].as<int>();
    auto tile_size = args["tile-size"].as<int>();
    TileManager::getInstance()->setOptions(tile_size, tile_size, max_tile_memory);

    auto image_size = args["size"].as<double>();
    auto rot_angle = args["rotation"].as<double>() / 180.0 * M_PI;
    auto scale = args["scale"].as<double>();
    auto shift_x = args["shift-x"].as<double>();
    auto shift_y = args["shift-y"].as<double>();
    auto model_size = args["model-size"].as<double>();
    if (model_size <= 0.) {
      model_size = image_size;
    }

    m_zero_point = args["zero-point"].as<double>();
    m_exp_time = args["exposure-time"].as<double>();

    std::vector<ConstantModel> constant_models;
    std::vector<TransformedModel> extended_models;
    std::vector<PointModel> point_models;

    std::shared_ptr<VariablePsf> vpsf;

    auto psf_filename = args["psf-file"].as<std::string>();
    if (psf_filename != "") {
      logger.info() << "Loading psf file: " << psf_filename;
      vpsf = PsfPluginConfig::readPsf(psf_filename);
    } else {
      vpsf = PsfPluginConfig::generateGaussianPsf(args["psf-fwhm"].as<double>(), args["psf-scale"].as<double>());
    }

    std::shared_ptr<CoordinateSystem> coordinate_system;
    auto copy_coordinate_system = args["copy-coordinate-system"].as<std::string>();
    if (copy_coordinate_system != "") {
      coordinate_system = std::make_shared<WCS>(copy_coordinate_system);
    } else {
      coordinate_system = std::make_shared<DummyWCS>(image_size, image_size, rot_angle, scale, shift_x, shift_y);
    }

    auto raster_model_size = model_size / vpsf->getPixelSampling() + std::max(vpsf->getWidth(), vpsf->getHeight());
    if (raster_model_size * raster_model_size > std::numeric_limits<int>::max()) {
      logger.fatal() << "The expected required memory for model rasterization exceeds the maximum size for an integer";
      logger.fatal() << "Please, either reduce the model size, the image size, or increase the PSF pixel scale";
      logger.fatal() << raster_model_size * raster_model_size << " > " << std::numeric_limits<int>::max();
      return Elements::ExitCode::NOT_OK;
    }

    // Generate a single PSF for the image
    const auto& vpsf_components = vpsf->getComponents();
    std::vector<double> psf_vals(vpsf_components.size());
    for (auto i = 0u; i < psf_vals.size(); ++i) {
      if (vpsf_components[i].name == "X_IMAGE" || vpsf_components[i].name == "Y_IMAGE") {
        psf_vals[i] = image_size / 2 - 1;
      }
      else {
        throw Elements::Exception() << "Unknown PSF component " << vpsf_components[i].name;
      }
    }

    // Generate and normalize the PSF
    auto p = vpsf->getPsf(psf_vals);
    auto psf_sum = std::accumulate(p->getData().begin(), p->getData().end(), 0.);
    p = VectorImage<SeFloat>::create(*MultiplyImage<SeFloat>::create(p, 1. / psf_sum));
    auto psf = std::make_shared<ImagePsf>(vpsf->getPixelSampling(), p);

    std::vector<TestImageSource> sources;

    std::string sources_filename = args["source-list"].as<std::string>();
    if (sources_filename != "") {
      logger.info() << "Loading sources from < " << sources_filename << " >...";
      auto loaded_sources = loadSourcesFromFile(sources_filename);
      sources.insert(sources.end(), loaded_sources.begin(), loaded_sources.end());
    }

    std::string sources_cat_filename = args["source-catalog"].as<std::string>();
    if (sources_cat_filename != "") {
      logger.info() << "Loading source catalog from < " << sources_cat_filename << " >...";
      auto loaded_sources = loadSourcesFromCatalog(sources_cat_filename);
      sources.insert(sources.end(), loaded_sources.begin(), loaded_sources.end());
    }

    int nb_of_random_sources = args["random-sources"].as<int>();
    if (nb_of_random_sources > 0) {
      logger.info() << "Adding " << nb_of_random_sources << " random source" << (nb_of_random_sources>1 ? "s" : "") << "...";
      auto random_sources = generateRandomSources(nb_of_random_sources, image_size * .1, image_size * .1, image_size * .9, image_size * .9);
      sources.insert(sources.end(), random_sources.begin(), random_sources.end());
    }

    std::string sources_save_filename = args["save-sources"].as<std::string>();
    if (sources_save_filename != "") {
      logger.info() << "Saving sources to < " << sources_save_filename << " >...";
      saveSources(sources, sources_save_filename);
    }

    logger.info("Transforming sources...");
    transformSources(sources, image_size, rot_angle, scale, shift_x, shift_y);

    logger.info("Creating source models...");
    for (const auto& source : sources) {
      addSource(point_models, extended_models, model_size, source, std::make_tuple(scale, 0, 0, scale));
    }

    logger.info("Rendering...");

    auto filename = args["output"].as<std::string>();
    auto target_image_source = std::make_shared<FitsImageSource<SeFloat>>(filename, image_size, image_size, coordinate_system);
    std::shared_ptr<WriteableImage<SeFloat>> target_image(WriteableBufferedImage<SeFloat>::create(target_image_source));

    if (args["disable-psf"].as<bool>()) {
      FrameModel<DummyPsf<std::shared_ptr<WriteableImage<SeFloat>>>, std::shared_ptr<WriteableImage<SeFloat>>> frame_model {
        pixel_scale,
        (std::size_t) image_size, (std::size_t) image_size,
        std::move(constant_models),
        std::move(point_models),
        std::move(extended_models),
      };
      frame_model.rasterToImage(target_image);
    } else {
      FrameModel<ImagePsf, std::shared_ptr<WriteableImage<SeFloat>>> frame_model {
        pixel_scale,
        (std::size_t) image_size, (std::size_t) image_size,
        std::move(constant_models),
        std::move(point_models),
        std::move(extended_models),
        *psf
      };
      frame_model.rasterToImage(target_image);
    }


    logger.info("Adding noise...");

    addPoissonNoise(target_image, args["gain"].as<double>());
    addBackgroundNoise(target_image, args["bg-level"].as<double>(), args["bg-sigma"].as<double>());

    logger.info("Adding saturation...");

    auto saturation_level = args["saturation"].as<double>();
    saturate(target_image, saturation_level);

    logger.info("Creating weight map...");

    auto weight_filename = args["output-weight"].as<std::string>();
    if (weight_filename != "") {
      auto weight_map_source = std::make_shared<FitsImageSource<SeFloat>>(weight_filename, image_size, image_size);
      auto weight_map = WriteableBufferedImage<SeFloat>::create(weight_map_source);
      for (int y = 0; y < image_size; ++y) {
        for (int x = 0; x < image_size; ++x) {
          weight_map->setValue(x, y, 1);
        }
      }

      logger.info("Adding bad pixels...");

      addBadPixels(weight_map, args["bad-pixels"].as<double>());
      addBadColumns(weight_map, args["bad-columns"].as<double>());

      for (int y = 0; y < target_image->getHeight(); y++) {
        for (int x = 0; x < target_image->getWidth(); x++) {
          if (weight_map->getValue(x, y) == 0) {
            target_image->setValue(x, y, saturation_level);
          }
        }
      }
    }

    logger.info("All done ^__^");
    return Elements::ExitCode::OK;
  }

private:
  boost::random::mt19937 m_rng { (unsigned int) time(NULL) } ;
  double m_zero_point, m_exp_time;

};

MAIN_FOR(TestImage)

