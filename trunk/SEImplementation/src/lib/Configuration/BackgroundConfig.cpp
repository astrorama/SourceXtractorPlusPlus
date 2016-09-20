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
      {BACKGROUND_VALUE.c_str(), po::value<SeFloat>()->default_value(0),
          "Background value to be subtracted from the detection image."}
  }}};
}

void BackgroundConfig::initialize(const UserValues& args) {
  m_background_value = args.find(BACKGROUND_VALUE)->second.as<SeFloat>();
}

} // SExtractor namespace
