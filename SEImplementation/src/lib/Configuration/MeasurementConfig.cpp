/*
 * MeasurementConfig.cpp
 *
 *  Created on: Nov 4, 2016
 *      Author: mschefer
 */

#include <iostream>

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>

#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string.hpp>

#include <yaml-cpp/yaml.h>

#include "SEFramework/Image/FitsReader.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/CoordinateSystem/WCS.h"

#include "SEImplementation/Configuration/MeasurementConfig.h"


using namespace Euclid::Configuration;

namespace po = boost::program_options;
namespace bfs = boost::filesystem;
namespace ba = boost::adaptors;

namespace SExtractor {

static const std::string MEASUREMENT_CONFIG_FILE { "measurement-config-file" };
static const std::string MEASUREMENT_IMAGES_GAIN { "measurement-images-gain" };
static const std::string MEASUREMENT_IMAGES_SATURATION { "measurement-images-saturation" };
static const std::string MEASUREMENT_IMAGES_FLUX_SCALE { "measurement-images-flux-scale" };
static const std::string MEASUREMENT_APERTURE_SIZES { "measurement-aperture-sizes" };

MeasurementConfig::MeasurementConfig(long manager_id) : Configuration(manager_id),
        m_override_gain(false),
        m_gain(0.0),
        m_override_saturation(false),
        m_saturation(0),
        m_override_flux_scale(false),
        m_flux_scale(1)
{
}

std::map<std::string, Configuration::OptionDescriptionList> MeasurementConfig::getProgramOptions() {
  return { {"Measurement config", {
      { MEASUREMENT_CONFIG_FILE.c_str(), po::value<std::string>()->default_value({}, ""),
          "Config file for measurement" },
      { MEASUREMENT_IMAGES_GAIN.c_str(), po::value<double>(),
          "Sets the gain for all measurement images (overrides FITS data)" },
      { MEASUREMENT_IMAGES_SATURATION.c_str(), po::value<double>(),
          "Sets the saturation for all measurement images (overrides FITS data)" },
      { MEASUREMENT_IMAGES_FLUX_SCALE.c_str(), po::value<double>(),
          "Sets the flux scalefor all measurement images (overrides FITS data)" }
  }}};
}

void MeasurementConfig::initialize(const UserValues& args) {
  auto filename = args.find(MEASUREMENT_CONFIG_FILE)->second.as<std::string>();

  if (args.find(MEASUREMENT_IMAGES_GAIN) != args.end()) {
    m_override_gain = true;
    m_gain = args.find(MEASUREMENT_IMAGES_GAIN)->second.as<double>();
  }
  if (args.find(MEASUREMENT_IMAGES_SATURATION) != args.end()) {
    m_override_saturation= true;
    m_saturation = args.find(MEASUREMENT_IMAGES_SATURATION)->second.as<double>();
  }
  if (args.find(MEASUREMENT_IMAGES_FLUX_SCALE) != args.end()) {
    m_override_flux_scale = true;
    m_flux_scale = args.find(MEASUREMENT_IMAGES_FLUX_SCALE)->second.as<double>();
  }


  if (filename != "") {
    m_yaml_config = YAML::LoadFile(filename);
    parseTree();
  }
}

void MeasurementConfig::parseTree() {
  std::cout << "### parseTree" << std::endl;

  for (auto iter : m_yaml_config) {
    for (auto node : iter) {
      if (node.first.as<std::string>() == "measurements-group") {
        parseMeasurementsGroup(node.second);
      }
    }
  }
}

void MeasurementConfig::parseMeasurementsGroup(const YAML::Node& image_group) {
  std::cout << "### parseMeasurementsGroup" << std::endl;

  auto group = std::make_shared<ImageGroup>();
  m_groups.push_back(group);

  try {
    group->setName(image_group["name"].as<std::string>());
  } catch(std::exception& e) {
  }

  for (auto node : image_group) {
    if (node.first.as<std::string>() == "image-files") {
      auto images = parseImageFiles(node.second);
      group->addImages(images);
    }
  }
}

std::set<unsigned int> MeasurementConfig::parseImageFiles(const YAML::Node& image_group) {
  std::cout << "### parseImageFiles" << std::endl;
  std::set<unsigned int> images;

  std::vector<std::string> image_filenames;
  std::vector<std::string> weight_filenames;
  std::vector<std::string> psf_filenames;

  for (auto node : image_group) {
    for (auto subnode : node) {
      if (subnode.first.as<std::string>() == "path") {
        auto file_paths = getFilenamesFromPath(subnode.second.as<std::string>());
        image_filenames.insert(image_filenames.end(), file_paths.begin(), file_paths.end());
      } else if (subnode.first.as<std::string>() == "weight-image-path") {
        auto file_paths = getFilenamesFromPath(subnode.second.as<std::string>());
        weight_filenames.insert(weight_filenames.end(), file_paths.begin(), file_paths.end());
      } else if (subnode.first.as<std::string>() == "psf-path") {
        auto file_paths = getFilenamesFromPath(subnode.second.as<std::string>());
        psf_filenames.insert(psf_filenames.end(), file_paths.begin(), file_paths.end());
      } else {
        std::cout << "Unknown node: " << node.first.as<std::string>() << "\n";
      }
    }
  }

  auto weight_iter = weight_filenames.begin();
  auto psf_iter = psf_filenames.begin();

  for (auto path : image_filenames) {
    std::cout << path << std::endl;

    std::string weight_path = (weight_iter != weight_filenames.end()) ? *(weight_iter++) : "";
    std::string psf_path = (psf_iter != psf_filenames.end()) ? *(psf_iter++) : "";
    auto image = addImage(path, weight_path, psf_path);
    images.insert(image);
  }

  return images;
}

std::vector<std::string> MeasurementConfig::getFilenamesFromPath(const std::string& path) const {
  auto filename = bfs::path(path);

  auto root_path = bfs::absolute(filename.parent_path());
  std::cout << ">>> " << filename << " ## " << root_path << std::endl;

  auto path_regexp = filename.filename().string();
  boost::replace_all(path_regexp, ".", "\\.");
  boost::replace_all(path_regexp, "*", ".*");
  const boost::regex filter(path_regexp);

  auto iter = boost::make_iterator_range(bfs::directory_iterator(root_path), {})
      | ba::filtered(static_cast<bool (*)(const bfs::path &)>(&bfs::is_regular_file))
      | ba::filtered([&](const bfs::path &path) {
          boost::smatch what;
          return boost::regex_match(path.filename().string(), what, filter);
      });

  std::vector<std::string> file_paths;
  for (auto path : iter) {
    file_paths.emplace_back(path.path().string());
  }

  std::sort(file_paths.begin(), file_paths.end());

  if (file_paths.empty()) {
    throw Elements::Exception() << "Failed to find any file matching for: " << path;
  }

  return file_paths;
}

unsigned int MeasurementConfig::addImage(const std::string filename, const std::string weight_filename, const std::string psf_filename) {
  std::cout << "adding: " << filename << " weight: " << weight_filename << " psf: " << psf_filename << std::endl;
  auto iter = m_loaded_images.find(filename);

  if (iter != m_loaded_images.end()) {
    return iter->second;
  } else {
    //auto image = FitsReader<MeasurementImage::PixelType>::readFile(filename);

    auto fits_image_source = std::make_shared<FitsImageSource<DetectionImage::PixelType>>(filename);
    std::shared_ptr<MeasurementImage> image = BufferedImage<DetectionImage::PixelType>::create(fits_image_source);

    auto coordinate_system = std::make_shared<WCS>(filename);

    std::cout << "w: " << image->getWidth() << " h: " << image->getHeight() << std::endl;

    // FIXME we should not be limited to one image/weight_map combination
    std::shared_ptr<WeightImage> weight_map = weight_filename.size() > 0 ? FitsReader<WeightImage::PixelType>::readFile(weight_filename) : nullptr;
    if (weight_map != nullptr) {
      std::cout << "w: " << weight_map->getWidth() << " h: " << weight_map->getHeight() << std::endl;

      weight_map = WeightImageConfig::convertWeightMap(weight_map, WeightImageConfig::WeightType::WEIGHT_TYPE_RMS);
    }

    // Default values
    double measurement_image_gain = 0, measurement_image_saturate = 0, flux_scale = 1.0;

    fits_image_source->readFitsKeyword("GAIN", measurement_image_gain);
    fits_image_source->readFitsKeyword("SATURATE", measurement_image_saturate);
    fits_image_source->readFitsKeyword("FLXSCALE", flux_scale);

    if (m_override_gain) {
      measurement_image_gain = m_gain;
    }
    if (m_override_saturation) {
      measurement_image_saturate = m_saturation;
    }
    if (m_override_flux_scale) {
      flux_scale = m_flux_scale;
    }

    if (flux_scale != 1.0) {
      image = MultiplyImage<MeasurementImage::PixelType>::create(image, flux_scale);
      measurement_image_saturate *= flux_scale;
    }

    m_measurement_images.push_back(std::move(image));
    m_coordinate_systems.push_back(coordinate_system);
    m_weight_images.push_back(std::move(weight_map));
    m_psfs_paths.push_back(psf_filename);
    m_absolute_weights.push_back(true); // FIXME we should have that in the config file

    m_gains.push_back(measurement_image_gain);
    m_saturation_levels.push_back(measurement_image_saturate);

    unsigned int image_index = m_measurement_images.size() - 1;

    m_loaded_images[filename] = image_index;

    return m_measurement_images.size() - 1;
  }
}

std::string MeasurementConfig::ImageGroup::getName() const {
  if (m_name == "") {
    std::stringstream group_name_stream;
    unsigned int default_group_nb = 0;
    group_name_stream << "group" << default_group_nb++;
    const_cast<MeasurementConfig::ImageGroup*>(this)->m_name = group_name_stream.str();
  }

  return m_name;
}

}
