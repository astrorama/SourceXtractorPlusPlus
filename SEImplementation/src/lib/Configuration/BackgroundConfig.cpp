/* 
 * @file BackgroundConfig.cpp
 * @author nikoapos
 */

#include "SEFramework/Image/SubtractImage.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string BACKGROUND_VALUE {"background-value" };

BackgroundConfig::BackgroundConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<DetectionImageConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> BackgroundConfig::getProgramOptions() {
  return { {"Detection image", {
      {BACKGROUND_VALUE.c_str(), po::value<double>(),
          "Background value to be subtracted from the detection image."}
  }}};
}

void BackgroundConfig::initialize(const UserValues& args) {
  if (args.count(BACKGROUND_VALUE) != 0) {
    auto bg_value = args.find(BACKGROUND_VALUE)->second.as<double>();
    getDependency<DetectionImageConfig>().addDecorateImageAction(
          [this, bg_value](std::shared_ptr<DetectionImage> image) {
            m_background_subtracted_image = std::make_shared<SubtractImage<DetectionImage::PixelType>>(image, bg_value);
            return m_background_subtracted_image;
          }
    );
  }
}

std::shared_ptr<DetectionImage> BackgroundConfig::getBackgroundSubtractedImage() const {
  if (getCurrentState() < State::FINAL) {
    throw Elements::Exception() << "getBackgroundSubtractedImage() call on not finalized BackgroundConfig";
  }
  return m_background_subtracted_image;
}

} // SExtractor namespace
