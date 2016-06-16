/**
 * @file src/lib/SegmentationConfig.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include <boost/algorithm/string.hpp>

#include "ElementsKernel/Exception.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/CatalogConfig.h"

#include "SEConfig/SegmentationConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string SEGMENTATION_ALGORITHM {"segmentation-algorithm" };

SegmentationConfig::SegmentationConfig(long manager_id) : Configuration(manager_id),
      m_selected_algorithm(Algorithm::UNKNOWN) {
}

std::map<std::string, Configuration::OptionDescriptionList> SegmentationConfig::getProgramOptions() {
  return { {"Detection image", {
      {SEGMENTATION_ALGORITHM.c_str(), po::value<std::string>()->default_value("LUTZ"),
          "Segmentation algorithm to be used. Currently LUTZ is the only choice"},
  }}};
}

void SegmentationConfig::initialize(const UserValues& args) {
  auto algorithm_name = args.find(SEGMENTATION_ALGORITHM)->second.as<std::string>();
  boost::algorithm::to_lower(algorithm_name);

  if (algorithm_name == "lutz") {
    m_selected_algorithm = Algorithm::LUTZ;
  } else {
    throw Elements::Exception("Unknown segmentation algorithm.");
  }
}

} // SEConfig namespace
