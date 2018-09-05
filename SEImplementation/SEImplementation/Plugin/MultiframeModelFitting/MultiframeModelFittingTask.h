/*
 * MultiframeModelFittingTask.h
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

class MultiframeModelFittingTask : public GroupTask {

public:
  MultiframeModelFittingTask(unsigned int max_iterations,
      std::vector<std::vector<int>> frame_indices_per_band);

  virtual ~MultiframeModelFittingTask();

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  std::shared_ptr<VectorImage<SeFloat>> createWeightImage(SourceGroupInterface& group, int frame_index) const;
  std::shared_ptr<VectorImage<SeFloat>> createImageCopy(SourceGroupInterface& group, int frame_index) const;

  bool isFrameValid(SourceGroupInterface& group, int frame_index) const;
  std::tuple<double, double, double, double> computeJacobianForFrame(SourceGroupInterface& group, int frame_index) const;

  // Task configuration
  unsigned int m_max_iterations;
  std::vector<std::vector<int>> m_frame_indices_per_band;

  std::map<int, std::shared_ptr<VectorImage<SeFloat>>> m_debug_images;
  std::map<int, std::shared_ptr<Image<SeFloat>>> m_residual_images;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_ */
