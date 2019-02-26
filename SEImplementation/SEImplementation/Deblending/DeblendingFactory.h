/* 
 * @file DeblendingFactory.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_DEBLENDINGFACTORY_H


#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Source/SourceFactory.h"

#include <SEImplementation/Configuration/DeblendStepConfig.h>

namespace SExtractor {

class DeblendingFactory : public Configurable {
  
public:
  
  DeblendingFactory(std::shared_ptr<SourceFactory> source_factory) :m_source_factory{source_factory} {
  }
  
  virtual ~DeblendingFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override {
    manager.registerConfiguration<DeblendStepConfig>();
  }

  void configure(Euclid::Configuration::ConfigManager& manager) override {
    m_steps = manager.getConfiguration<DeblendStepConfig>().getSteps(m_source_factory);
  }
  
  std::unique_ptr<Deblending> createDeblending() const {
    return std::unique_ptr<Deblending>(new Deblending(m_steps));
  }
  
private:
  
  std::shared_ptr<SourceFactory> m_source_factory;
  std::vector<std::shared_ptr<DeblendStep>> m_steps;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_DEBLENDINGFACTORY_H */

