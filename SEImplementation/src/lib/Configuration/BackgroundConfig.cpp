/* 
 * @file BackgroundConfig.cpp
 * @author nikoapos
 */

#include "SEFramework/Image/SubtractImage.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string BACKGROUND_VALUE {"background-value" };
static const std::string THRESHOLD_VALUE {"detection-threshold" };

BackgroundConfig::BackgroundConfig(long manager_id) :
    Configuration(manager_id),
    m_background_level_absolute(false),
    m_background_level(0),
    m_detection_threshold_absolute(false),
    m_detection_threshold(0) {
}

std::map<std::string, Configuration::OptionDescriptionList> BackgroundConfig::getProgramOptions() {
  return { {"Detection image", {
      {BACKGROUND_VALUE.c_str(), po::value<SeFloat>(),
          "Background value to be subtracted from the detection image."},
      {THRESHOLD_VALUE.c_str(), po::value<SeFloat>()->default_value((1.5)),
          "Detection threshold above the background."},
  }}};
}

void BackgroundConfig::initialize(const UserValues& args) {
  if (args.find(BACKGROUND_VALUE) != args.end()) {
    m_background_level_absolute = true;
    m_background_level = args.find(BACKGROUND_VALUE)->second.as<SeFloat>();
  }
  if (args.find(THRESHOLD_VALUE) != args.end()) {
    m_detection_threshold_absolute = true;
    m_detection_threshold = args.find(THRESHOLD_VALUE)->second.as<SeFloat>();
  }
}

} // SExtractor namespace
