/*
 * CleaningConfig.cpp
 *
 *  Created on: 2019 M02 6
 *      Author: mschefer
 */

#include <SEImplementation/Configuration/DeblendStepConfig.h>
#include "SEFramework/Pipeline/Deblending.h"

#include "SEImplementation/Deblending/Cleaning.h"

#include "SEImplementation/Configuration/CleaningConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string USE_CLEANING {"use-cleaning"};

CleaningConfig::CleaningConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<DeblendStepConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> CleaningConfig::getProgramOptions() {
  return { {"Cleaning", {
      {USE_CLEANING.c_str(), po::bool_switch(),
         "Enables the cleaning of sources (removes false detections near bright objects)"}
  }}};
}

void CleaningConfig::initialize(const UserValues& args) {
  if (args.at(USE_CLEANING).as<bool>()) {
    getDependency<DeblendStepConfig>().addDeblendStepCreator(
        [](std::shared_ptr<SourceFactory> ) {
          return std::make_shared<Cleaning>();
        }
    );
  }
}

} // SExtractor namespace



