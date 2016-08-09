/* 
 * @file AttractorsPartitionConfig.cpp
 * @author nikoapos
 */

#include "SEImplementation/Configuration/AttractorsPartitionConfig.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"

#include "SEImplementation/Partition/AttractorsPartitionStep.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string USE_ATTRACTORS_PARTITION {"use-attractors-partition"};

AttractorsPartitionConfig::AttractorsPartitionConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PartitionStepConfig>();
}

auto AttractorsPartitionConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Extraction", {
      {USE_ATTRACTORS_PARTITION.c_str(), po::bool_switch(),
              "Enables the use of attractors for partitioning"}
  }}};
}

void AttractorsPartitionConfig::initialize(const UserValues& args) {
  if (args.at(USE_ATTRACTORS_PARTITION).as<bool>()) {
    getDependency<PartitionStepConfig>().addPartitionStepCreator(
            [](std::shared_ptr<SourceFactory> source_factory) {
              return std::make_shared<AttractorsPartitionStep>(source_factory);
            }
    );
  }
}

} // SExtractor namespace