/* 
 * @file DeblendStepConfg.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_DEBLENDSTEPCONFG_H
#define _SEIMPLEMENTATION_DEBLENDSTEPCONFG_H

#include <vector>
#include <functional>

#include "Configuration/Configuration.h"

#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Source/SourceFactory.h"

namespace SExtractor {

class DeblendStepConfig : public Euclid::Configuration::Configuration {
  
public:
  
  using DeblendStepCreator = std::function<std::shared_ptr<DeblendStep>(std::shared_ptr<SourceFactory>)>;
  
  DeblendStepConfig(long manager_id);
  
  virtual ~DeblendStepConfig() = default;
  
  void addDeblendStepCreator(DeblendStepCreator step_creator);
  
  std::vector<std::shared_ptr<DeblendStep>> getSteps(std::shared_ptr<SourceFactory> source_factory) const;
  
private:
  
  std::vector<DeblendStepCreator> m_deblend_step_creators;
  
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_DEBLENDSTEPCONFG_H */

