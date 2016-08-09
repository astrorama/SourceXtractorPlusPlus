/*
 * SimpleSourceFactory.h
 *
 *  Created on: Aug 9, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SIMPLESOURCEFACTORY_H_
#define _SEFRAMEWORK_SOURCE_SIMPLESOURCEFACTORY_H_

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Source/SimpleSource.h"

namespace SExtractor {

class SimpleSourceFactory : public SourceFactory {

public:

  SimpleSourceFactory() {}

  virtual std::shared_ptr<SourceInterface> createSource() const override {
    return std::make_shared<SimpleSource>();
  }
};

}

#endif /* _SEFRAMEWORK_SOURCE_SIMPLESOURCEFACTORY_H_ */
