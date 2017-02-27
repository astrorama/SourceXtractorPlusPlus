/* 
 * @file MinAreaPartitionConfig.cpp
 * @author nikoapos
 */

#include <iostream>

#include "SEImplementation/Configuration/MinAreaPartitionConfig.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"
#include "SEImplementation/Configuration/AttractorsPartitionConfig.h"
#include "SEImplementation/Configuration/MultiThresholdPartitionConfig.h"

#include "SEImplementation/Partition/MinAreaPartitionStep.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string DETECT_MINAREA {"detect-minarea"};

MinAreaPartitionConfig::MinAreaPartitionConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PartitionStepConfig>();
  // The following line guarantees that the Attractors partitioning will be done
  // before the min area filtering
  //ConfigManager::getInstance(manager_id).registerDependency<MinAreaPartitionConfig, AttractorsPartitionConfig>();
  //ConfigManager::getInstance(manager_id).registerDependency<MultiThresholdPartitionConfig, AttractorsPartitionConfig>();
}

auto MinAreaPartitionConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Extraction", {
      {DETECT_MINAREA.c_str(), po::value<int>(), "min. # of pixels above threshold"}
  }}};
}

void MinAreaPartitionConfig::initialize(const UserValues& args) {
  if (args.count(DETECT_MINAREA) != 0) {
    std::cout << "MinAreaPartitionConfig" << std::endl;
    auto min_pixel_count = args.at(DETECT_MINAREA).as<int>();
    getDependency<PartitionStepConfig>().addPartitionStepCreator(
            [min_pixel_count](std::shared_ptr<SourceFactory>) {
              return std::make_shared<MinAreaPartitionStep>(min_pixel_count);
            }
    );
  }
}

} // SExtractor namespace
