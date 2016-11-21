/*
 * MeasurementConfig.cpp
 *
 *  Created on: Nov 4, 2016
 *      Author: mschefer
 */

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>

#include "SEFramework/Image/FitsReader.h"
#include "SEImplementation/Configuration/MeasurementConfig.h"

using namespace Euclid::Configuration;

namespace po = boost::program_options;
namespace pt = boost::property_tree;
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
    pt::read_json(filename, m_property_tree);
    parseTree();
  }

//  for (auto group : m_groups) {
//    auto group_images = group->getMeasurementImages();
//    m_measurement_images.insert(m_measurement_images.begin(), group_images.begin(), group_images.end());
//  }
}

void MeasurementConfig::parseTree() {
  std::cout << "### parseTree" << std::endl;

  AperturePhotometryOptions ap_options;
  ap_options.updateOptions(m_property_tree);

  for (auto& property : m_property_tree) {
    if (property.first == "image_group") {
      parseImageGroup(property.second, ap_options);
    }
  }
}

void MeasurementConfig::parseImageGroup(const boost::property_tree::ptree& image_group, AperturePhotometryOptions ap_options) {
  std::cout << "### parseImageGroup" << std::endl;

  ap_options.updateOptions(image_group);

  auto group = std::make_shared<ImageGroup>();
  group->setAperturePhotometryOptions(ap_options);
  m_groups.push_back(group);

  for (auto property : image_group) {
    if (property.first == "image_files") {
      auto images = parseImageFiles(property.second);
      group->addImages(images);
    }
  }
}

std::set<unsigned int> MeasurementConfig::parseImageFiles(const boost::property_tree::ptree& image_group) {
  std::cout << "### parseImageFiles" << std::endl;

  auto filename = bfs::path(image_group.get<std::string>("image_path"));
  auto root_path = bfs::absolute(filename.root_path());
  std::cout << ">>>" << filename << "##" << root_path << std::endl;

  const boost::regex filter(filename.filename().string());

  auto iter = boost::make_iterator_range(bfs::directory_iterator(root_path), {})
      | ba::filtered(static_cast<bool (*)(const bfs::path &)>(&bfs::is_regular_file))
      | ba::filtered([&](const bfs::path &path){
          boost::smatch what;
          return boost::regex_match(path.filename().string(), what, filter);
      });

  // FIXME sort images by lexicographic order

  std::set<unsigned int> images;
  for (auto path : iter) {
    std::cout << path << std::endl;

    auto image = addImage(std::move(FitsReader<MeasurementImage::PixelType>::readFile(path.path().string())));
    images.insert(image);
  }

  return images;
}

unsigned int MeasurementConfig::addImage(std::shared_ptr<MeasurementImage> image) {
  // fixme check it's not already there
  m_measurement_images.push_back(image);
  return m_measurement_images.size() - 1;
}


void MeasurementConfig::AperturePhotometryOptions::updateOptions(const boost::property_tree::ptree& image_group) {
  try {
    auto aperture_options = image_group.get_child("aperture_photometry");
    for (auto property : aperture_options) {
      if (property.first == "aggregate" && property.first == "mean") { // FIXME
        m_aggregate_type = AggregateType::Mean;
      }
    }
  } catch (std::exception& e) { // FIXME specific exception
  }
}


}
