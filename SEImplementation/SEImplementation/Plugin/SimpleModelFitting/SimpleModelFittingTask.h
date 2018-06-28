/*
 * SimpleModelFittingTask.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASK_H_

#include "SEFramework/Task/GroupTask.h"

#include "SEImplementation/Image/ImagePsf.h"

namespace SExtractor {

class SimpleModelFittingTask : public GroupTask {

public:
  SimpleModelFittingTask(unsigned int max_iterations)
    : m_max_iterations(max_iterations) {}

  virtual ~SimpleModelFittingTask() = default;

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  unsigned int m_max_iterations;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASK_H_ */
