/*
 * WeightImageConfig.cpp
 *
 *  Created on: Oct 7, 2016
 *      Author: mschefer
 */

#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/FitsReader.h"

#include "SEImplementation/Configuration/WeightImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string WEIGHT_IMAGE {"weight-image" };

WeightImageConfig::WeightImageConfig(long manager_id) : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> WeightImageConfig::getProgramOptions() {
  return { {"Weight image", {
      {WEIGHT_IMAGE.c_str(), po::value<std::string>(),
          "Path to a fits format image to be used as weight image."}
  }}};
}

void WeightImageConfig::initialize(const UserValues& args) {
  m_weight_image = FitsReader<DetectionImage::PixelType>::readFile(args.find(WEIGHT_IMAGE)->second.as<std::string>());
}

}

