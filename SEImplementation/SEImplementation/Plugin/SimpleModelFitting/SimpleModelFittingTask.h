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
  SimpleModelFittingTask(std::shared_ptr<ImagePsf> psf, unsigned int max_iterations)
    : m_psf(psf),
      m_max_iterations(max_iterations) {}

  virtual ~SimpleModelFittingTask() = default;

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  std::shared_ptr<ImagePsf> m_psf;
  unsigned int m_max_iterations;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASK_H_ */
