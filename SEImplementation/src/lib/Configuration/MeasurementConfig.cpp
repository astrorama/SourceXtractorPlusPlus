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
#include "SEImplementation/Configuration/PsfConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/CoordinateSystem/WCS.h"

#include "SEImplementation/Configuration/MeasurementConfig.h"


using namespace Euclid::Configuration;

namespace po = boost::program_options;
namespace bfs = boost::filesystem;
namespace ba = boost::adaptors;

namespace SExtractor {

static const std::string MEASUREMENT_CONFIG_FILE { "measurement-config-file" };

MeasurementConfig::MeasurementConfig(long manager_id) : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> MeasurementConfig::getProgramOptions() {
  return { {"Measurement config", {
      { MEASUREMENT_CONFIG_FILE.c_str(), po::value<std::string>()->default_value({}, ""),
          "Config file for measurement" }
  }}};
}

void MeasurementConfig::initialize(const UserValues& args) {
  auto filename = args.find(MEASUREMENT_CONFIG_FILE)->second.as<std::string>();

  if (filename != "") {
    m_yaml_config = YAML::LoadFile(filename);
    parseTree();
  }
}

void MeasurementConfig::parseTree() {
  std::cout << "### parseTree" << std::endl;

  AperturePhotometryOptions ap_options;
  ap_options.updateOptions(m_yaml_config);

  for (auto node : m_yaml_config) {
    if (node.first.as<std::string>() == "measurements-group") {
      parseMeasurementsGroup(node.second, ap_options);
    }
  }
}

void MeasurementConfig::parseMeasurementsGroup(const YAML::Node& image_group, AperturePhotometryOptions ap_options) {
  std::cout << "### parseMeasurementsGroup" << std::endl;

  ap_options.updateOptions(image_group);

  auto group = std::make_shared<ImageGroup>();
  group->setAperturePhotometryOptions(ap_options);
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
    if (node.first.as<std::string>() == "path") {
      auto file_paths = getFilenamesFromPath(node.second.as<std::string>());
      image_filenames.insert(image_filenames.end(), file_paths.begin(), file_paths.end());
    } else if (node.first.as<std::string>() == "weight-image-path") {
      auto file_paths = getFilenamesFromPath(node.second.as<std::string>());
      weight_filenames.insert(weight_filenames.end(), file_paths.begin(), file_paths.end());
    } else if (node.first.as<std::string>() == "psf-path") {
      auto file_paths = getFilenamesFromPath(node.second.as<std::string>());
      psf_filenames.insert(psf_filenames.end(), file_paths.begin(), file_paths.end());
    } else {
      std::cout << "Unknown node: " << node.first.as<std::string>() << "\n";
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

  auto root_path = bfs::absolute(filename.root_path());
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

  return file_paths;
}

unsigned int MeasurementConfig::addImage(const std::string filename, const std::string weight_filename, const std::string psf_filename) {
  std::cout << "adding: " << filename << " weight: " << weight_filename << " psf: " << psf_filename << std::endl;
  auto iter = m_loaded_images.find(filename);

  if (iter != m_loaded_images.end()) {
    return iter->second;
  } else {
    auto image = FitsReader<MeasurementImage::PixelType>::readFile(filename);
    auto coordinate_system = std::make_shared<WCS>(filename);

    std::cout << "w: " << image->getWidth() << " h: " << image->getHeight() << std::endl;

    // FIXME we should not be limited to one image/weight_map combination
    std::shared_ptr<WeightImage> weight_map = weight_filename.size() > 0 ? FitsReader<WeightImage::PixelType>::readFile(weight_filename) : nullptr;
    if (weight_map != nullptr) {
      std::cout << "w: " << weight_map->getWidth() << " h: " << weight_map->getHeight() << std::endl;

      weight_map = WeightImageConfig::convertWeightMap(weight_map, WeightImageConfig::WeightType::WEIGHT_TYPE_RMS);
    }

    std::shared_ptr<ImagePsf> psf = psf_filename.size() > 0 ? PsfConfig::readPsf(psf_filename) : nullptr;

    m_measurement_images.push_back(std::move(image));
    m_coordinate_systems.push_back(coordinate_system);
    m_weight_images.push_back(std::move(weight_map));
    m_psfs.push_back(std::move(psf));
    m_absolute_weights.push_back(true); // FIXME we should have that in the config file

    unsigned int image_index = m_measurement_images.size() - 1;

    m_loaded_images[filename] = image_index;

    return m_measurement_images.size() - 1;
  }
}

void MeasurementConfig::AperturePhotometryOptions::updateOptions(const YAML::Node& image_group) {
  try {
    auto aperture_options = image_group["aperture-photometry"];
//    for (auto node : aperture_options) {
//    }

    try {
      const auto& aperture_size_node = aperture_options["size"];

      if (aperture_size_node.Type() == YAML::NodeType::Sequence) {
        for (auto aperture_size : aperture_size_node) {
          m_aperture_sizes.push_back(aperture_size.as<double>());
        }
      } else if (aperture_size_node.Type() == YAML::NodeType::Scalar) {
        m_aperture_sizes.push_back(aperture_size_node.as<double>());
      } else {
        // fixme throw
      }

    } catch(std::exception& e) {
    }

  } catch (std::exception& e) { // FIXME specific exception
    std::cout << "updateOptions exception" << std::endl;
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
