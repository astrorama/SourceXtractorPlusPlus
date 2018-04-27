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

static const std::string DETECTION_IMAGE { "detection-image" };
static const std::string DETECTION_IMAGE_GAIN { "detection-image-gain" };
static const std::string DETECTION_IMAGE_SATURATION { "detection-image-saturation" };
static const std::string DETECTION_IMAGE_INTERPOLATION { "detection-image-interpolation" };

DetectionImageConfig::DetectionImageConfig(long manager_id) : Configuration(manager_id),
    m_gain(0), m_saturation(0), m_interpolate(false) {
}

std::map<std::string, Configuration::OptionDescriptionList> DetectionImageConfig::getProgramOptions() {
  return { {"Detection image", {
      {DETECTION_IMAGE.c_str(), po::value<std::string>(),
          "Path to a fits format image to be used as detection image."},
      {DETECTION_IMAGE_GAIN.c_str(), po::value<double>(),
          "Detection image gain in e-/ADU (0 = infinite gain)"},
      {DETECTION_IMAGE_SATURATION.c_str(), po::value<double>(),
          "Detection image saturation level (0 = no saturation)"},
      {DETECTION_IMAGE_INTERPOLATION.c_str(), po::value<bool>()->default_value(true),
          "Interpolate bad pixels in detection image"}
  }}};
}

void DetectionImageConfig::initialize(const UserValues& args) {
  auto fits_image_source = std::make_shared<FitsImageSource<DetectionImage::PixelType>>(
      args.find(DETECTION_IMAGE)->second.as<std::string>());
  m_detection_image = BufferedImage<DetectionImage::PixelType>::create(fits_image_source);
  m_coordinate_system = std::make_shared<WCS>(args.find(DETECTION_IMAGE)->second.as<std::string>());

  double detection_image_gain = 0, detection_image_saturate = 0;
  fits_image_source->readFitsKeyword("GAIN", detection_image_gain);
  fits_image_source->readFitsKeyword("SATURATE", detection_image_saturate);

  if (args.find(DETECTION_IMAGE_GAIN) != args.end()) {
    m_gain = args.find(DETECTION_IMAGE_GAIN)->second.as<double>();
  }

  if (args.find(DETECTION_IMAGE_SATURATION) != args.end()) {
    m_saturation = args.find(DETECTION_IMAGE_SATURATION)->second.as<double>();
  }

  m_interpolate = args.find(DETECTION_IMAGE_INTERPOLATION)->second.as<bool>();
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



