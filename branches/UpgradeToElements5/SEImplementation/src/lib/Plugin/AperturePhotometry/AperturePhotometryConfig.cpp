/*
 * AperturePhotometryConfig.cpp
 *
 *  Created on: Sep 26, 2016
 *      Author: mschefer
 */

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
      {APERTURE_SIZE.c_str(), po::value<std::vector<SeFloat>>()->default_value({}, ""),
          "Aperture size"},
  }}};
}

void AperturePhotometryConfig::preInitialize(const UserValues& args) {
  m_apertures = args.at(APERTURE_SIZE).as<std::vector<SeFloat>>();
}

auto AperturePhotometryConfig::getApertures() const -> const std::vector<SeFloat>& {
  return m_apertures;
}

} // SExtractor namespace






