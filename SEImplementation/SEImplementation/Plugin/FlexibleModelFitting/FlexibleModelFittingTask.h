/*
 * FlexibleModelFittingTask.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASK_H_

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Task/GroupTask.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingFrame.h"

namespace SExtractor {

class FlexibleModelFittingTask : public GroupTask {

public:
  FlexibleModelFittingTask(unsigned int max_iterations);
  virtual ~FlexibleModelFittingTask();

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  bool isFrameValid(SourceGroupInterface& group, int frame_index) const;
  std::tuple<double, double, double, double> computeJacobianForFrame(
      SourceGroupInterface& group, int frame_index) const;
  std::shared_ptr<VectorImage<SeFloat>> createImageCopy(
      SourceGroupInterface& group, int frame_index) const;
  std::shared_ptr<VectorImage<SeFloat>> createWeightImage(
      SourceGroupInterface& group, int frame_index) const;

  void createParameters() const;

  // Task configuration
  unsigned int m_max_iterations;

  std::vector<std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  std::vector<std::shared_ptr<FlexibleModelFittingFrame>> m_frames;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASK_H_ */
