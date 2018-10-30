/*
 * AperturePhotometryConfig.cpp
 *
 *  Created on: Sep 26, 2016
 *      Author: mschefer
 */

#include <boost/tokenizer.hpp>
#include "SEUtils/Types.h"

#include "Configuration/ProgramOptionsHelper.h"
#include "SEFramework/Image/FitsReader.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryConfig.h"

namespace po = boost::program_options;
using poh = Euclid::Configuration::ProgramOptionsHelper;

namespace SExtractor {

namespace {

const std::string APERTURE_SIZE {"aperture-size"};

}

auto AperturePhotometryConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Aperture photometry options", {
      {APERTURE_SIZE.c_str(), po::value<std::string>()->default_value("5.0"),
          "Aperture size"},
  }}};
}

void AperturePhotometryConfig::preInitialize(const UserValues& args) {
  // Boost doesn't seem to work well with multiple values on a configuration file, so
  // we do the parsing here ourselves
  auto aperture_str = args.at(APERTURE_SIZE).as<std::string>();
  boost::char_separator<char> sep(" \t;,");
  boost::tokenizer<boost::char_separator<char>> tok(aperture_str, sep);

  for (auto v : tok) {
    m_apertures.push_back(boost::lexical_cast<SeFloat>(v));
  }
}

auto AperturePhotometryConfig::getApertures() const -> const std::vector<SeFloat>& {
  return m_apertures;
}

} // SExtractor namespace






