/*
 * MeasurementFrameGroupRectangleTask.h
 *
 *  Created on: Sep 5, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASK_H_


#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

class MeasurementFrameGroupRectangleTask : public GroupTask {
public:

  virtual ~MeasurementFrameGroupRectangleTask() = default;

  MeasurementFrameGroupRectangleTask(unsigned instance): m_instance{instance} {}

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:
  unsigned m_instance;
};

} // end SExtractor


#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASK_H_
