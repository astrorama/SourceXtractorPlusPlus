/*
 * MultiThresholdPartitionConfig.cpp
 *
 *  Created on: Jan 18, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Configuration/MultiThresholdPartitionConfig.h"
#include "SEImplementation/Configuration/MinAreaPartitionConfig.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"

#include "SEImplementation/Partition/MultiThresholdPartitionStep.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string MULTI_THRESHOLDS {"thresholds-nb"};

MultiThresholdPartitionConfig::MultiThresholdPartitionConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PartitionStepConfig>();

  ConfigManager::getInstance(manager_id).registerDependency<MultiThresholdPartitionConfig, MinAreaPartitionConfig>();

}

auto MultiThresholdPartitionConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Multi-thresholding", {
      {MULTI_THRESHOLDS.c_str(), po::value<int>(), "# of thresholds"}
  }}};

  return {};
}

void MultiThresholdPartitionConfig::initialize(const UserValues& args) {
  if (args.count(MULTI_THRESHOLDS) != 0) {
    std::cout << "MultiThresholdPartitionConfig" << std::endl;

    auto threshold_nb = args.at(MULTI_THRESHOLDS).as<int>();

    getDependency<PartitionStepConfig>().addPartitionStepCreator(
      [](std::shared_ptr<SourceFactory> source_factory) {
        return std::make_shared<MultiThresholdPartitionStep>(source_factory);
      }
    );
  }

}

} // SExtractor namespace
