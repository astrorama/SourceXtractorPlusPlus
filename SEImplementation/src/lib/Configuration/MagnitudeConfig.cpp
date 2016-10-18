/*
 * MagnitudeConfig.cpp
 *
 *  Created on: Sep 29, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/MagnitudeConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string MAG_ZEROPOINT {"magnitude-zeropoint"};

MagnitudeConfig::MagnitudeConfig(long manager_id) :
    Configuration(manager_id),
    m_magnitude_zero_point(0) {
}

auto MagnitudeConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Magnitude", {
      {MAG_ZEROPOINT.c_str(), po::value<double>()->default_value(0.0), "Magnitude zero point calibration"}
  }}};
}

void MagnitudeConfig::initialize(const UserValues& args) {
  if (args.count(MAG_ZEROPOINT) != 0) {
    m_magnitude_zero_point = SeFloat(args.at(MAG_ZEROPOINT).as<double>());
  }
}

}

