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
static const std::string DETECTION_IMAGE_INTERPOLATION_GAP { "detection-image-interpolation-gap" };

DetectionImageConfig::DetectionImageConfig(long manager_id) : Configuration(manager_id),
    m_gain(0), m_saturation(0), m_interpolation_gap(0) {
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
          "Interpolate bad pixels in detection image"},
      {DETECTION_IMAGE_INTERPOLATION_GAP.c_str(), po::value<int>()->default_value(5),
          "Maximum number if pixels to interpolate over"}
  }}};
}

void DetectionImageConfig::initialize(const UserValues& args) {
  // Normally we would define this one as required, but then --list-output-properties would be
  // unusable unless we also specify --detection-image, which is not very intuitive.
  // For this reason, we check for its existence here
  if (args.find(DETECTION_IMAGE) == args.end()) {
    throw Elements::Exception() << "'--" << DETECTION_IMAGE << "' is required but missing";
  }

  auto fits_image_source = std::make_shared<FitsImageSource<DetectionImage::PixelType>>(
      args.find(DETECTION_IMAGE)->second.as<std::string>());
  m_detection_image = BufferedImage<DetectionImage::PixelType>::create(fits_image_source);
  m_coordinate_system = std::make_shared<WCS>(args.find(DETECTION_IMAGE)->second.as<std::string>());

  double detection_image_gain = 0, detection_image_saturate = 0;
  fits_image_source->readFitsKeyword("GAIN", detection_image_gain);
  fits_image_source->readFitsKeyword("SATURATE", detection_image_saturate);
  //fits_image_source->readFitsKeyword("GAIN", m_gain);
  //fits_image_source->readFitsKeyword("SATURATE", m_saturation);

  if (args.find(DETECTION_IMAGE_GAIN) != args.end()) {
    m_gain = args.find(DETECTION_IMAGE_GAIN)->second.as<double>();
  }
  else {
    m_gain = detection_image_gain;
  }

  if (args.find(DETECTION_IMAGE_SATURATION) != args.end()) {
    m_saturation = args.find(DETECTION_IMAGE_SATURATION)->second.as<double>();
  }
  else {
    m_saturation = detection_image_saturate;
  }

  m_interpolation_gap = args.find(DETECTION_IMAGE_INTERPOLATION)->second.as<bool>() ?
      std::max(0, args.find(DETECTION_IMAGE_INTERPOLATION_GAP)->second.as<int>()) : 0;
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



