/*
 * Output.h
 *
 *  Created on: Jun 23, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_OUTPUT_H_
#define _SEFRAMEWORK_PIPELINE_OUTPUT_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SExtractor {

class Output :
    public Observer<std::shared_ptr<SourceInterface>>,
    public Observer<std::shared_ptr<SourceGroupInterface>> {

public:

  virtual ~Output() = default;

  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source) override {
    outputSource(*source);
  }

  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) override {
    for (auto& source: *source_group) {
      outputSource(source);
    }
  }

  virtual void outputSource(const SourceInterface& source) = 0;
};

}

#endif /* _SEFRAMEWORK_PIPELINE_OUTPUT_H_ */
