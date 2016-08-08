/*
 * SourceWithOnDemandPropertiesFactory.h
 *
 *  Created on: Aug 8, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEWITHONDEMANDPROPERTIESFACTORY_H_
#define _SEFRAMEWORK_SOURCE_SOURCEWITHONDEMANDPROPERTIESFACTORY_H_

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"

namespace SExtractor {

class SourceWithOnDemandPropertiesFactory : public SourceFactory {
public:
  SourceWithOnDemandPropertiesFactory(std::shared_ptr<TaskProvider> task_provider) :
        m_task_provider(task_provider) {}

  virtual std::shared_ptr<SourceInterface> createSource() const override {
    return std::make_shared<SourceWithOnDemandProperties>(m_task_provider);
  }

private:
  std::shared_ptr<TaskProvider> m_task_provider;
};

}


#endif /* _SEFRAMEWORK_SOURCE_SOURCEWITHONDEMANDPROPERTIESFACTORY_H_ */
