/*
 * MultiThresholdPartitionConfig.cpp
 *
 *  Created on: Jan 18, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/MultiThresholdPartitionConfig.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"

#include "SEImplementation/Partition/MultiThresholdPartitionStep.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string MULTI_THRESHOLDS {"thresholds-nb"};

MultiThresholdPartitionConfig::MultiThresholdPartitionConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PartitionStepConfig>();
}

auto MultiThresholdPartitionConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Multi-thresholding", {
      {MULTI_THRESHOLDS.c_str(), po::value<int>(), "# of thresholds"}
  }}};

  return {};
}

void MultiThresholdPartitionConfig::initialize(const UserValues& args) {
  getDependency<PartitionStepConfig>().addPartitionStepCreator(
    [](std::shared_ptr<SourceFactory> source_factory) {
      return std::make_shared<MultiThresholdPartitionStep>(source_factory);
    }
  );
}

} // SExtractor namespace
