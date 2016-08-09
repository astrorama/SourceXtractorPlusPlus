/* 
 * @file PartitionStepConfig.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_PARTITIONSTEPCONFIG_H
#define _SEIMPLEMENTATION_PARTITIONSTEPCONFIG_H

#include <vector>
#include <functional>

#include "Configuration/Configuration.h"

#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Source/SourceFactory.h"

namespace SExtractor {

class PartitionStepConfig : public Euclid::Configuration::Configuration {
  
public:
  
  using PartitionStepCreator = std::function<std::shared_ptr<PartitionStep>(std::shared_ptr<SourceFactory>)>;
  
  PartitionStepConfig(long manager_id);
  
  virtual ~PartitionStepConfig() = default;
  
  void addPartitionStepCreator(PartitionStepCreator step_creator);
  
  std::vector<std::shared_ptr<PartitionStep>> getSteps(std::shared_ptr<SourceFactory> source_factory) const;
  
private:
  
  std::vector<PartitionStepCreator> m_partition_step_creators;
  
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PARTITIONSTEPCONFIG_H */

