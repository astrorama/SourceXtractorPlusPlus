/**
 * @file src/lib/Configuration/OutputConfig.cpp
 * @date 06/23/16
 * @author mschefer
 */

#include <boost/tokenizer.hpp>

#include "Configuration/ConfigManager.h"
#include "Configuration/Configuration.h"
#include "SEImplementation/Configuration/OutputConfig.h"
#include "SEImplementation/Property/PixelCentroid.h"
#include "SEImplementation/Property/PixelBoundaries.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string OUTPUT_COLUMNS {"output-columns"};
static const std::string OUTPUT_PIXEL_CENTROID {"output-pixel-centroid"};
static const std::string OUTPUT_PIXEL_BOUNDARIES {"output-pixel-boundaries"};

OutputConfig::OutputConfig(long manager_id) : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> OutputConfig::getProgramOptions() {
  return { {"Output configuration", {
      {OUTPUT_COLUMNS.c_str(), po::value<std::string>()->default_value(""),
          "Comma-separated list of output columns"},
      {OUTPUT_PIXEL_CENTROID.c_str(), po::bool_switch(),
          "Flag for adding the centroid (in pixels) to the output catalog"},
      {OUTPUT_PIXEL_BOUNDARIES.c_str(), po::bool_switch(),
          "Flag for adding the boundaries (in pixels) to the output catalog"}
  }}};
}

void OutputConfig::initialize(const UserValues& args) {
  auto& output_columns_string = args.at(OUTPUT_COLUMNS).as<std::string>();

  boost::char_separator<char> separator(",");
  boost::tokenizer<boost::char_separator<char>> tok(output_columns_string, separator);

  std::copy(tok.begin(), tok.end(), back_inserter(m_output_columns));
  
  m_pixel_centroid = (args.at(OUTPUT_PIXEL_CENTROID).as<bool>());
  m_pixel_boundaries = (args.at(OUTPUT_PIXEL_BOUNDARIES).as<bool>());
}

void OutputConfig::setEnabledOutputs(OutputRegistry& output_registry) {
  if (m_pixel_centroid) {
    output_registry.enableOutput<PixelCentroid>();
  }
  if (m_pixel_boundaries) {
    output_registry.enableOutput<PixelBoundaries>();
  }
}

} // SEImplementation namespace



