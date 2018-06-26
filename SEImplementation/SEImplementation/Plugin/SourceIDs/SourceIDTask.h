/*
 * SourceIDTask.h
 *
 *  Created on: May 30, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Property/SourceId.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"

namespace SExtractor {

class SourceIDTask : public SourceTask {

public:
  virtual ~SourceIDTask() = default;

  SourceIDTask() {}

  virtual void computeProperties(SourceInterface& source) const override {
    auto detection_id = source.getProperty<SourceId>().getDetectionId();
    source.setProperty<SourceID>(getNewId(), detection_id);
  }

private:
  static unsigned int getNewId() {
    static unsigned int s_id  = 1;
    return s_id++;
  }

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASK_H_ */
