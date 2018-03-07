/**
 * @file src/lib/DetectionImageConfig.cpp
 * @date 06/06/16
 * @author mschefer
 */
#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/FitsReader.h"

#include "SEImplementation/CoordinateSystem/WCS.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string DETECTION_IMAGE {"detection-image" };
static const std::string DETECTION_IMAGE_GAIN {"detection-image-gain" };
static const std::string DETECTION_IMAGE_SATURATION {"detection-image-saturation" };

DetectionImageConfig::DetectionImageConfig(long manager_id) : Configuration(manager_id),
    m_gain(0), m_saturation(0) {
}

std::map<std::string, Configuration::OptionDescriptionList> DetectionImageConfig::getProgramOptions() {
  return { {"Detection image", {
      {DETECTION_IMAGE.c_str(), po::value<std::string>(),
          "Path to a fits format image to be used as detection image."},
      {DETECTION_IMAGE_GAIN.c_str(), po::value<double>()->default_value(0.0),
          "Detection image gain in e-/ADU (0 = infinite gain)"},
      {DETECTION_IMAGE_SATURATION.c_str(), po::value<double>()->default_value(0.0),
          "Detection image saturation level (0 = no saturation)"}
  }}};
}

void DetectionImageConfig::initialize(const UserValues& args) {
  m_detection_image = FitsReader<DetectionImage::PixelType>::readFile(args.find(DETECTION_IMAGE)->second.as<std::string>());

  m_coordinate_system = std::make_shared<WCS>(args.find(DETECTION_IMAGE)->second.as<std::string>());
  m_gain = args.find(DETECTION_IMAGE_GAIN)->second.as<double>();
  m_saturation = args.find(DETECTION_IMAGE_SATURATION)->second.as<double>();
}

std::shared_ptr<DetectionImage> DetectionImageConfig::getDetectionImage() const {
  if (getCurrentState() < State::FINAL) {
    throw Elements::Exception() << "getDetectionImage() call on not finalized DetectionImageConfig";
  }
  return m_detection_image;
}

std::shared_ptr<CoordinateSystem> DetectionImageConfig::getCoordinateSystem() const {
  if (getCurrentState() < State::FINAL) {
    throw Elements::Exception() << "getCoordinateSystem() call on not finalized DetectionImageConfig";
  }
  return m_coordinate_system;
}

} // SExtractor namespace



