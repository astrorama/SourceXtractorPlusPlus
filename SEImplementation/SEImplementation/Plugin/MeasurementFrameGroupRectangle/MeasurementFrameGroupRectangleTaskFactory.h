/*
 * MeasurementFrameGroupRectangleTaskFactory.h
 *
 *  Created on: Sep 5, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASKFACTORY_H_


#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class MeasurementFrameGroupRectangleTaskFactory : public TaskFactory {

public:

  virtual ~MeasurementFrameGroupRectangleTaskFactory() = default;

  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

};

} // end SExtractor


#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASKFACTORY_H_
