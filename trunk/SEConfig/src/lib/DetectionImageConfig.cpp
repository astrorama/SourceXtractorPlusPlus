/**
 * @file src/lib/DetectionImageConfig.cpp
 * @date 06/06/16
 * @author mschefer
 */
#include "Configuration/ConfigManager.h"
#include "Configuration/CatalogConfig.h"

#include "SEFramework/Image/FitsImage.h"
#include "SEFramework/Image/SubtractImage.h"

#include "SEConfig/DetectionImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

using namespace SEFramework;

namespace SEConfig {

static const std::string DETECTION_IMAGE {"detection-image" };
static const std::string BACKGROUND_VALUE {"background-value" };

DetectionImageConfig::DetectionImageConfig(long manager_id) : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> DetectionImageConfig::getProgramOptions() {
  return { {"Detection image", {
      {DETECTION_IMAGE.c_str(), po::value<std::string>(),
          "Path to a fits format image to be used as detection image."},
      {BACKGROUND_VALUE.c_str(), po::value<double>(),
          "Background value to be subtracted from the detection image."}
  }}};
}

void DetectionImageConfig::initialize(const UserValues& args) {
  auto fits_image = std::make_shared<FitsImage>(args.find(DETECTION_IMAGE)->second.as<std::string>());
  m_detection_image = std::make_shared<SubtractImage>(
      fits_image, args.find(BACKGROUND_VALUE)->second.as<double>());
}


} // SEConfig namespace



