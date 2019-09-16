/* 
 * @file MinAreaPartitionConfig.cpp
 * @author nikoapos
 */

#include "SEImplementation/Configuration/MinAreaPartitionConfig.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"

#include "SEImplementation/Partition/MinAreaPartitionStep.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string DETECT_MINAREA {"detection-minimum-area"};

MinAreaPartitionConfig::MinAreaPartitionConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PartitionStepConfig>();
}

auto MinAreaPartitionConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Extraction", {
      {DETECT_MINAREA.c_str(), po::value<int>()->default_value(3), "min. # of pixels above threshold"}
  }}};
}

void MinAreaPartitionConfig::initialize(const UserValues& args) {
  if (args.count(DETECT_MINAREA) != 0) {
    auto min_pixel_count = args.at(DETECT_MINAREA).as<int>();
    getDependency<PartitionStepConfig>().addPartitionStepCreator(
            [min_pixel_count](std::shared_ptr<SourceFactory>) {
              return std::make_shared<MinAreaPartitionStep>(min_pixel_count);
            }
    );
  }
}

} // SExtractor namespace
