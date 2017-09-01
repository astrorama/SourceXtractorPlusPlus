/**
 * @file src/lib/SExtractorConfig.cpp
 * @date 07/20/16
 * @author nikoapos
 */

#include "SEMain/SExtractorConfig.h"
#include "SEMain/PluginConfig.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"

#include "SEImplementation/Configuration/BackgroundConfig.h"
#include "SEImplementation/Configuration/MinAreaPartitionConfig.h"
#include "SEImplementation/Configuration/AttractorsPartitionConfig.h"
#include "SEImplementation/Configuration/MultiThresholdPartitionConfig.h"

namespace po = boost::program_options;

namespace SExtractor {

SExtractorConfig::SExtractorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<DetectionImageConfig>();
  declareDependency<WeightImageConfig>();

  declareDependency<BackgroundConfig>();
  declareDependency<MinAreaPartitionConfig>();
  declareDependency<AttractorsPartitionConfig>();
  declareDependency<MultiThresholdPartitionConfig>();
  // The following dependency is not used but it is included so its options
  // are included in the help message
  declareDependency<PluginConfig>();
}

} // SExtractor namespace



