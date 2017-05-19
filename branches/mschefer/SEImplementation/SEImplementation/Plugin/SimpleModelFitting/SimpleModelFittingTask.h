/*
 * SimpleModelFittingTask.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASK_H_
#define SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASK_H_

#include "SEFramework/Image/VectorImage.h"

#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

class SimpleModelFittingTask : public GroupTask {

public:
  SimpleModelFittingTask(unsigned int max_iterations) : m_max_iterations(max_iterations) {}

  virtual ~SimpleModelFittingTask() = default;

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  unsigned int m_max_iterations;

  // check image
  std::shared_ptr<VectorImage<DetectionImage::PixelType>> m_check_image;



};

}




#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTINGTASK_H_ */
