/*
 * MeasurementNeighbourInfoTaskFactory.h
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class MeasurementNeighbourInfoTaskFactory: public TaskFactory {
public:
  std::shared_ptr<Task> createTask(const PropertyId& property_id) const;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOTASKFACTORY_H_
