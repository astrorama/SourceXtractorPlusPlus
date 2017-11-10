/* 
 * @file BackgroundConfig.cpp
 * @author nikoapos
 */

#include "SEFramework/Image/SubtractImage.h"
#include "SEImplementation/Configuration/SE2BackgroundConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string CELLSIZE_VALUE {"cellsize-value" };
static const std::string SMOOTHINGBOX_VALUE {"smoothing-box-value" };

SE2BackgroundConfig::SE2BackgroundConfig(long manager_id) :
    Configuration(manager_id),
    m_cell_size(0),
    m_smoothing_box(0) {
}

std::map<std::string, Configuration::OptionDescriptionList> SE2BackgroundConfig::getProgramOptions() {
  return { {"Background modelling", {
      {CELLSIZE_VALUE.c_str(), po::value<std::string>()->default_value(std::string("")),
          "Background mesh cell size to determine a value."},
      {SMOOTHINGBOX_VALUE.c_str(), po::value<std::string>()->default_value(std::string("")),
          "Background median filter size"},
  }}};
}

void SE2BackgroundConfig::initialize(const UserValues& args) {
  if (args.find(CELLSIZE_VALUE) != args.end()) {
    m_cell_size = args.find(CELLSIZE_VALUE)->second.as<std::string>();
  }
  if (args.find(SMOOTHINGBOX_VALUE) != args.end()) {
    m_smoothing_box = args.find(SMOOTHINGBOX_VALUE)->second.as<std::string>();
  }
}

} // SExtractor namespace
