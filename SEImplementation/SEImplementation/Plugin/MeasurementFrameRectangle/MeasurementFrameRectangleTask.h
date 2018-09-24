/*
 * MeasurementFrameRectangleTask.h
 *
 *  Created on: Sep 24, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASK_H_


#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class MeasurementFrameRectangleTask : public SourceTask {
public:

  virtual ~MeasurementFrameRectangleTask() = default;

  MeasurementFrameRectangleTask(unsigned instance): m_instance{instance} {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  unsigned m_instance;
};

} // end SExtractor


#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASK_H_
