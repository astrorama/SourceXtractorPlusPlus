/*
 * NeighbourInfoTaskFactory.h
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class NeighbourInfoTaskFactory: public TaskFactory {
public:
  std::shared_ptr<Task> createTask(const PropertyId& property_id) const;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_DETECTIONNEIGHBOURINFO_DETECTIONNEIGHBOURINFOTASKFACTORY_H_
