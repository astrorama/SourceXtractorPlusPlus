/*
 * WorldCentroidTask.h
 *
 *  Created on: Nov 21, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_WORLDCENTROID_WORLDCENTROIDTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_WORLDCENTROID_WORLDCENTROIDTASK_H_

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class WorldCentroidTask : public SourceTask {

public:

  virtual ~WorldCentroidTask() = default;

  virtual void computeProperties(SourceInterface& source) const override;

private:

};

}

#endif
