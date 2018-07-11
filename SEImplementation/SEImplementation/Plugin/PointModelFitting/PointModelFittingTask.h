/*
 * PointModelFittingTask.h
 *
 *  Created on: July 11, 2018
 *      Author: Alejandro Álvarez
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGTASK_H_

#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

class PointModelFittingTask: public GroupTask {
public:
  PointModelFittingTask(unsigned int max_iterations): m_max_iterations(max_iterations) {}

  virtual ~PointModelFittingTask() = default;

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:
  unsigned int m_max_iterations;
};

} // SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_POINTMODELFITTING_POINTMODELFITTINGTASK_H_ */
