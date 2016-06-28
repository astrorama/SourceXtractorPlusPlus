/*
 * Output.h
 *
 *  Created on: Jun 23, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_OUTPUT_H_
#define _SEFRAMEWORK_PIPELINE_OUTPUT_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Source/EntangledSourceGroup.h"

namespace SExtractor {

class Output :
    public Observer<std::shared_ptr<Source>>,
    public Observer<std::shared_ptr<EntangledSourceGroup>> {

public:

  virtual ~Output() = default;

  virtual void handleMessage(const std::shared_ptr<Source>& source) override {
    outputSource(*source);
  }

  virtual void handleMessage(const std::shared_ptr<EntangledSourceGroup>& source_group) override {
    for (auto& source: source_group->getSources()) {
      outputSource(*source);
    }
  }

  virtual void outputSource(const SourceInterface& source) = 0;
};

}

#endif /* _SEFRAMEWORK_PIPELINE_OUTPUT_H_ */
