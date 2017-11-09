/*
 * MultiframeModelFittingTask.h
 *
 *  Created on: Sep 12, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_


#include "SEFramework/Image/VectorImage.h"
#include "SEImplementation/Image/ImagePsf.h"

#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

class MultiframeModelFittingTask : public GroupTask {

public:
  MultiframeModelFittingTask(unsigned int max_iterations,
      std::vector<std::vector<int>> frame_indices_per_band, std::vector<std::shared_ptr<ImagePsf>> psfs);

  virtual ~MultiframeModelFittingTask() = default;

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  std::shared_ptr<VectorImage<SeFloat>> createWeightImage(
      SourceGroupInterface& group, int width, int height, PixelCoordinate offset, int frame_index) const;


  unsigned int m_max_iterations;

  //std::vector<int> m_frame_indices;
  std::vector<std::vector<int>> m_frame_indices_per_band;
  std::vector<std::shared_ptr<ImagePsf>> m_psfs;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_ */
