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
static const std::string CLEANING_MINAREA {"cleaning-minarea"};

CleaningConfig::CleaningConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<DeblendStepConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> CleaningConfig::getProgramOptions() {
  return { {"Cleaning", {
      {USE_CLEANING.c_str(), po::bool_switch(),
         "Enables the cleaning of sources (removes false detections near bright objects)"},
      {CLEANING_MINAREA.c_str(), po::value<unsigned int>()->default_value(3), "min. # of pixels above threshold"}
  }}};
}

void CleaningConfig::initialize(const UserValues& args) {
  auto min_area = args.at(CLEANING_MINAREA).as<unsigned int>();
  if (args.at(USE_CLEANING).as<bool>()) {
    getDependency<DeblendStepConfig>().addDeblendStepCreator(
        [min_area](std::shared_ptr<SourceFactory> source_factory) {
          return std::make_shared<Cleaning>(source_factory, min_area);
        }
    );
  }
}

} // SExtractor namespace



