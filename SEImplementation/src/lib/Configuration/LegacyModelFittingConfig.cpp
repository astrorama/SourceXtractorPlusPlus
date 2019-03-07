/*
 * ModelFittingConfig.cpp
 *
 *  Created on: May 17, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/LegacyModelFittingConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string MFIT_MAX_ITERATIONS {"model-fitting-iterations"};

LegacyModelFittingConfig::LegacyModelFittingConfig(long manager_id) : Configuration(manager_id) {
}

auto LegacyModelFittingConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Model Fitting", {
      {MFIT_MAX_ITERATIONS.c_str(), po::value<int>()->default_value(1000), "Maximum number of iterations allowed for model fitting"},
  }}};

  return {};
}

void LegacyModelFittingConfig::initialize(const UserValues& args) {
  m_max_iterations = args.at(MFIT_MAX_ITERATIONS).as<int>();
  if (m_max_iterations <= 0) {
    throw Elements::Exception() << "Invalid " << MFIT_MAX_ITERATIONS << " value: " << m_max_iterations;
  }
}

} /* namespace SExtractor */


