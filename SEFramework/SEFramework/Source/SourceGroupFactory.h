/* 
 * @file SourceGroupFactory.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SOURCEGROUPFACTORY_H
#define _SEFRAMEWORK_SOURCEGROUPFACTORY_H

#include <memory>
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SExtractor {

class SourceGroupFactory {
public:
  virtual ~SourceGroupFactory() = default;

  virtual std::shared_ptr<SourceGroupInterface> createSourceGroup() const = 0;
};

}

#endif /* _SEFRAMEWORK_SOURCEGROUPFACTORY_H */

