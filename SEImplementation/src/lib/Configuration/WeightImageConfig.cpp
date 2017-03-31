/*
 * WeightImageConfig.cpp
 *
 *  Created on: Oct 7, 2016
 *      Author: mschefer
 */

#include <limits>
#include <boost/algorithm/string.hpp>

#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/FitsReader.h"

#include "SEImplementation/Configuration/WeightImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string WEIGHT_IMAGE {"weight-image" };
static const std::string WEIGHT_TYPE {"weight-type" };

WeightImageConfig::WeightImageConfig(long manager_id) : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> WeightImageConfig::getProgramOptions() {
  return { {"Weight image", {
      {WEIGHT_IMAGE.c_str(), po::value<std::string>()->default_value(""),
          "Path to a fits format image to be used as weight image."},
      {WEIGHT_TYPE.c_str(), po::value<std::string>()->default_value("NONE"),
          "Weight image type."}
  }}};
}

void WeightImageConfig::initialize(const UserValues& args) {
  auto weight_image_filename = args.find(WEIGHT_IMAGE)->second.as<std::string>();
  if (weight_image_filename != "") {
    m_weight_image = FitsReader<WeightImage::PixelType>::readFile(weight_image_filename);
  }

  auto weight_type_name = boost::to_upper_copy(args.at(WEIGHT_TYPE).as<std::string>());
  if (weight_type_name == "NONE") {
    m_weight_type = WeightType::WEIGHT_TYPE_NONE;
  } else if (weight_type_name == "BACKGROUND") {
    m_weight_type = WeightType::WEIGHT_TYPE_FROM_BACKGROUND;
  } else if (weight_type_name == "RMS") {
    m_weight_type = WeightType::WEIGHT_TYPE_ABS_RMS;
  } else if (weight_type_name == "VARIANCE") {
    m_weight_type = WeightType::WEIGHT_TYPE_REL_VARIANCE;
  } else if (weight_type_name == "WEIGHT") {
    m_weight_type = WeightType::WEIGHT_TYPE_REL_WEIGHT;
  } else {
    throw Elements::Exception() << "Unknown weight map type : " << weight_type_name;
  }

  if (m_weight_image != nullptr) {
    convertWeightMap(m_weight_type);
  }


}

void WeightImageConfig::convertWeightMap(WeightType weight_type) {
  auto new_image = std::make_shared<VectorImage<WeightImage::PixelType>>(m_weight_image->getWidth(), m_weight_image->getHeight());

  switch (weight_type) {
  default:
  case WeightType::WEIGHT_TYPE_NONE:
  case WeightType::WEIGHT_TYPE_FROM_BACKGROUND:
    m_weight_image.reset();
    break;
  case WeightType::WEIGHT_TYPE_ABS_RMS:
    for (int y = 0; y < m_weight_image->getHeight(); y++) {
      for (int x = 0; x < m_weight_image->getWidth(); x++) {
        auto value = m_weight_image->getValue(x, y);
        new_image->setValue(x, y, value * value);
      }
    }
    m_weight_image = new_image;
    break;
  case WeightType::WEIGHT_TYPE_REL_VARIANCE:
    for (int y = 0; y < m_weight_image->getHeight(); y++) {
      for (int x = 0; x < m_weight_image->getWidth(); x++) {
        new_image->setValue(x, y, m_weight_image->getValue(x, y));
      }
    }
    m_weight_image = new_image;
    break;
  case WeightType::WEIGHT_TYPE_REL_WEIGHT:
    for (int y = 0; y < m_weight_image->getHeight(); y++) {
      for (int x = 0; x < m_weight_image->getWidth(); x++) {
        auto value = m_weight_image->getValue(x, y);
        if (value > 0) {
          new_image->setValue(x, y, 1 / value);
        } else {
          new_image->setValue(x, y, std::numeric_limits<WeightImage::PixelType>::max());
        }
      }
    }
    m_weight_image = new_image;
    break;
  }

}

}

