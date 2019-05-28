/*
 * SnrLevelConfig.cpp
 *
 *  Created on: May 28, 2019
 *      Author: mkuemmel@usm.lmu.de
 */

#include <Configuration/ProgramOptionsHelper.h>
#include "SEImplementation/Plugin/SnrLevel/SnrLevelConfig.h"

namespace po = boost::program_options;
using Euclid::Configuration::Configuration;

namespace SExtractor {

static const std::string SNR_LEVEL {"snr-level" };

std::map<std::string, Configuration::OptionDescriptionList> SnrLevelConfig::getProgramOptions() {
  return {{"SNR Level", {
    {SNR_LEVEL.c_str(), po::value<double>()->default_value(0.0), "The SNR level to assemble pixel counts"}
  }}};
}

void SnrLevelConfig::initialize(const UserValues &args) {
  m_snr_level = args.find(SNR_LEVEL)->second.as<double>();
}

const double& SnrLevelConfig::getSnrLevel() const {
  return m_snr_level;
}

} // end SExtractor
