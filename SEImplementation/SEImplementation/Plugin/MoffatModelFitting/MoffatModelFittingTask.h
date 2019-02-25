/*
 * MoffatModelFittingTask.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASK_H_

#include "SEFramework/Task/SourceTask.h"

#include "SEImplementation/Image/ImagePsf.h"

namespace SExtractor {

class MoffatModelFittingTask : public SourceTask {

public:
  MoffatModelFittingTask(unsigned int max_iterations)
    : m_max_iterations(max_iterations) {}

  virtual ~MoffatModelFittingTask() = default;

  virtual void computeProperties(SourceInterface& source) const override;

private:

  unsigned int m_max_iterations;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASK_H_ */
