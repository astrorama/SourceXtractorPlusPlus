/*
 * NeighbourInfoTask.h
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOTASK_H_

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class NeighbourInfoTask: public SourceTask {
public:
  virtual ~NeighbourInfoTask() = default;

  void computeProperties(SourceInterface& source) const override;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOTASK_H_
