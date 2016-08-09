/* 
 * @file PartitionFactory.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_PARTITIONFACTORY_H
#define _SEIMPLEMENTATION_PARTITIONFACTORY_H

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Source/SourceFactory.h"

#include "SEImplementation/Configuration/PartitionStepConfig.h"

namespace SExtractor {

class PartitionFactory : public Configurable {
  
public:
  
  PartitionFactory(std::shared_ptr<SourceFactory> source_factory) :m_source_factory{source_factory} {
  }
  
  virtual ~PartitionFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override {
    manager.registerConfiguration<PartitionStepConfig>();
  }

  void configure(Euclid::Configuration::ConfigManager& manager) override {
    m_steps = manager.getConfiguration<PartitionStepConfig>().getSteps(m_source_factory);
  }
  
  std::shared_ptr<Partition> getPartition() const {
    return std::make_shared<Partition>(m_steps);
  }
  
private:
  
  std::shared_ptr<SourceFactory> m_source_factory;
  std::vector<std::shared_ptr<PartitionStep>> m_steps;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PARTITIONFACTORY_H */

