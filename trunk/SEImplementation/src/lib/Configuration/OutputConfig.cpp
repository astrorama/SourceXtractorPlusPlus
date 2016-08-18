/**
 * @file src/lib/Configuration/OutputConfig.cpp
 * @date 06/23/16
 * @author mschefer
 */

#include <sstream>

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Configuration/OutputConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string OUTPUT_FILE {"output-file"};
static const std::string OUTPUT_PROPERTIES {"output-properties"};

OutputConfig::OutputConfig(long manager_id) : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> OutputConfig::getProgramOptions() {
  return { {"Output configuration", {
      {OUTPUT_FILE.c_str(), po::value<std::string>()->default_value(""),
          "The file to store the output catalog"},
          {OUTPUT_PROPERTIES.c_str(), po::value<std::string>()->default_value(""),
          "The output properties to add in the output catalog"}
  }}};
}

void OutputConfig::initialize(const UserValues& args) {
  m_out_file = args.at(OUTPUT_FILE).as<std::string>();
  
  std::stringstream properties_str {args.at(OUTPUT_PROPERTIES).as<std::string>()};
  std::string name;
  while (std::getline(properties_str, name, ',')) {
    m_optional_properties.emplace_back(name);
  }
}

std::string OutputConfig::getOutputFile() {
  return m_out_file;
}

const std::vector<std::string> OutputConfig::getOptionalProperties() {
  return m_optional_properties;
}

} // SEImplementation namespace



