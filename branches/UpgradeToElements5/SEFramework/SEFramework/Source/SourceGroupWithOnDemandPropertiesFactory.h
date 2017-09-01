/* 
 * @file SourceGroupWithOnDemandPropertiesFactory.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SOURCEGROUPWITHONDEMANDPROPERTIESFACTORY_H
#define _SEFRAMEWORK_SOURCEGROUPWITHONDEMANDPROPERTIESFACTORY_H

#include "SEFramework/Source/SourceGroupFactory.h"
#include "SEFramework/Source/SourceGroupWithOnDemandProperties.h"

namespace SExtractor {

/**
 * @class SourceGroupWithOnDemandPropertiesFactory
 * @brief A SourceGroupFactory implementation that produces a SourceGroupWithOnDemandProperties
 */

class SourceGroupWithOnDemandPropertiesFactory : public SourceGroupFactory {
  
public:
  
  SourceGroupWithOnDemandPropertiesFactory(std::shared_ptr<TaskProvider> task_provider) :
        m_task_provider(task_provider) {}

  virtual std::shared_ptr<SourceGroupInterface> createSourceGroup() const override {
    return std::make_shared<SourceGroupWithOnDemandProperties>(m_task_provider);
  }
  
private:
  
  std::shared_ptr<TaskProvider> m_task_provider;
  
};

}

#endif /* _SEFRAMEWORK_SOURCEGROUPWITHONDEMANDPROPERTIESFACTORY_H */

