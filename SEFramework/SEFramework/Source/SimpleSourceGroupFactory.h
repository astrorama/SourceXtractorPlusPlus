/* 
 * @file SimpleSourceGroupFactory.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SIMPLESOURCEGROUPFACTORY_H
#define _SEFRAMEWORK_SIMPLESOURCEGROUPFACTORY_H

#include "SEFramework/Source/SourceGroupFactory.h"
#include "SEFramework/Source/SimpleSourceGroup.h"

namespace SExtractor {

class SimpleSourceGroupFactory : public SourceGroupFactory {
  
public:

  virtual std::shared_ptr<SourceGroupInterface> createSourceGroup() const override {
    return std::make_shared<SimpleSourceGroup>();
  }
  
};

}

#endif /* _SEFRAMEWORK_SIMPLESOURCEGROUPFACTORY_H */

