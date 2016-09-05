/**
 * @file src/lib/DetectionImageConfig.cpp
 * @date 06/06/16
 * @author mschefer
 */
#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/FitsReader.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string DETECTION_IMAGE {"detection-image" };

DetectionImageConfig::DetectionImageConfig(long manager_id) : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> DetectionImageConfig::getProgramOptions() {
  return { {"Detection image", {
      {DETECTION_IMAGE.c_str(), po::value<std::string>(),
          "Path to a fits format image to be used as detection image."}
  }}};
}

void DetectionImageConfig::initialize(const UserValues& args) {
  m_original_image = FitsReader<DetectionImage::PixelType>::readFile(args.find(DETECTION_IMAGE)->second.as<std::string>());
}

void DetectionImageConfig::postInitialize(const UserValues&) {
  m_detection_image = m_original_image;
  for (auto& action : m_decorate_action_list) {
    m_detection_image = action(m_detection_image);
  }
}


void DetectionImageConfig::addDecorateImageAction(DecorateImageAction action) {
  if (getCurrentState() >= State::FINAL) {
    throw Elements::Exception() << "getOriginalImage() call on finalized DetectionImageConfig";
  }
  m_decorate_action_list.emplace_back(action);
}

std::shared_ptr<DetectionImage> DetectionImageConfig::getOriginalImage() const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getOriginalImage() call on uninitialized DetectionImageConfig";
  }
  return m_original_image;
}

std::shared_ptr<DetectionImage> DetectionImageConfig::getDetectionImage() const {
  if (getCurrentState() < State::FINAL) {
    throw Elements::Exception() << "getDetectionImage() call on not finalized DetectionImageConfig";
  }
  return m_detection_image;
}

} // SExtractor namespace



