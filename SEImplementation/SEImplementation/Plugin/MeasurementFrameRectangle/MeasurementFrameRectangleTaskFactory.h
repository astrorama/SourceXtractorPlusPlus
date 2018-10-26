/*
 * MeasurementFrameRectangleTaskFactory.h
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASKFACTORY_H_


#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class MeasurementFrameRectangleTaskFactory : public TaskFactory {

public:

  virtual ~MeasurementFrameRectangleTaskFactory() = default;

  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

};

} // end SExtractor


#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASKFACTORY_H_
