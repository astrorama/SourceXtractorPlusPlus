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
static const std::string WEIGHT_ABSOLUTE {"weight-absolute" };
static const std::string WEIGHT_SCALING {"weight-scaling" };
static const std::string WEIGHT_THRESHOLD {"weight-threshold" };

WeightImageConfig::WeightImageConfig(long manager_id) :
    Configuration(manager_id),
    m_weight_type(WeightType::WEIGHT_TYPE_FROM_BACKGROUND),
    m_absolute_weight(false),
    m_weight_scaling(1),
    m_weight_threshold(0) {}

std::map<std::string, Configuration::OptionDescriptionList> WeightImageConfig::getProgramOptions() {
  return { {"Weight image", {
      {WEIGHT_IMAGE.c_str(), po::value<std::string>()->default_value(""),
          "Path to a fits format image to be used as weight image."},
      {WEIGHT_ABSOLUTE.c_str(), po::value<bool>()->default_value(false),
          "Is the weight map provided as absolute values or relative to background."},
      {WEIGHT_TYPE.c_str(), po::value<std::string>()->default_value("background"),
          "Weight image type [background|rms|variance|weight]."},
      {WEIGHT_SCALING.c_str(), po::value<double>()->default_value(1.0),
          "Weight map scaling factor."},
      {WEIGHT_THRESHOLD.c_str(), po::value<double>(),
          "Threshold for pixels to be considered bad pixels. In same units as weight map."},
  }}};
}

void WeightImageConfig::initialize(const UserValues& args) {
  m_absolute_weight = args.find(WEIGHT_ABSOLUTE)->second.as<bool>();
  auto weight_image_filename = args.find(WEIGHT_IMAGE)->second.as<std::string>();
  if (weight_image_filename != "") {
    m_weight_image = FitsReader<WeightImage::PixelType>::readFile(weight_image_filename);
  }

  auto weight_type_name = boost::to_upper_copy(args.at(WEIGHT_TYPE).as<std::string>());
  if (weight_type_name == "BACKGROUND") {
    m_weight_type = WeightType::WEIGHT_TYPE_FROM_BACKGROUND;
  } else if (weight_type_name == "RMS") {
    m_weight_type = WeightType::WEIGHT_TYPE_RMS;
  } else if (weight_type_name == "VARIANCE") {
    m_weight_type = WeightType::WEIGHT_TYPE_VARIANCE;
  } else if (weight_type_name == "WEIGHT") {
    m_weight_type = WeightType::WEIGHT_TYPE_WEIGHT;
  } else {
    throw Elements::Exception() << "Unknown weight map type : " << weight_type_name;
  }

  m_weight_scaling = args.find(WEIGHT_SCALING)->second.as<double>();

  if (m_weight_image != nullptr) {
    m_weight_image = convertWeightMap(m_weight_image, m_weight_type, m_weight_scaling);
  }

  if (args.count(WEIGHT_THRESHOLD) != 0) {
    auto threshold = args.find(WEIGHT_THRESHOLD)->second.as<double>();
    switch (m_weight_type) {
      default:
      case WeightType::WEIGHT_TYPE_FROM_BACKGROUND:
      case WeightType::WEIGHT_TYPE_RMS:
        m_weight_threshold = threshold * threshold;
        break;
      case WeightType::WEIGHT_TYPE_VARIANCE:
        m_weight_threshold = threshold;
        break;
      case WeightType::WEIGHT_TYPE_WEIGHT:
        if (threshold>0)
          m_weight_threshold = 1.0 / threshold;
        else
          m_weight_threshold = std::numeric_limits<WeightImage::PixelType>::max();
        break;
    }
  } else {
    m_weight_threshold = std::numeric_limits<WeightImage::PixelType>::max();
  }
}

std::shared_ptr<WeightImage> WeightImageConfig::convertWeightMap(std::shared_ptr<WeightImage> weight_image, WeightType weight_type, WeightImage::PixelType scaling) {
  auto new_image = VectorImage<WeightImage::PixelType>::create(weight_image->getWidth(), weight_image->getHeight());

  switch (weight_type) {
    default:
    case WeightType::WEIGHT_TYPE_FROM_BACKGROUND:
      return nullptr;
    case WeightType::WEIGHT_TYPE_RMS:
      for (int y = 0; y < weight_image->getHeight(); y++) {
        for (int x = 0; x < weight_image->getWidth(); x++) {
          auto value = weight_image->getValue(x, y) * scaling;
          new_image->setValue(x, y, value * value);
        }
      }
      return new_image;
    case WeightType::WEIGHT_TYPE_VARIANCE:
      for (int y = 0; y < weight_image->getHeight(); y++) {
        for (int x = 0; x < weight_image->getWidth(); x++) {
          new_image->setValue(x, y, weight_image->getValue(x, y) * scaling);
        }
      }
      return new_image;
    case WeightType::WEIGHT_TYPE_WEIGHT:
      for (int y = 0; y < weight_image->getHeight(); y++) {
        for (int x = 0; x < weight_image->getWidth(); x++) {
          auto value = weight_image->getValue(x, y) * scaling;
          if (value > 0) {
            new_image->setValue(x, y, 1.0 / value);
          } else {
            new_image->setValue(x, y, std::numeric_limits<WeightImage::PixelType>::max());
          }
        }
      }
      return new_image;
  }
}

}

