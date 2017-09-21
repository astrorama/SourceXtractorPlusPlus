/*
 * MultiframeModelFittingTask.h
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_


#include "SEFramework/Image/VectorImage.h"
#include "ModelFitting/Image/OpenCvPsf.h"

#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

class MultiframeModelFittingTask : public GroupTask {

public:
  MultiframeModelFittingTask(std::shared_ptr<ModelFitting::OpenCvPsf> psf, unsigned int max_iterations)
    : m_psf(psf),
      m_max_iterations(max_iterations) {}

  virtual ~MultiframeModelFittingTask() = default;

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  std::shared_ptr<ModelFitting::OpenCvPsf> m_psf;
  unsigned int m_max_iterations;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_ */
