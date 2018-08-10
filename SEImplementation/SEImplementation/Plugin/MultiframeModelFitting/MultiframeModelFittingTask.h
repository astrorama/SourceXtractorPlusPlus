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

  virtual ~MultiframeModelFittingTask();

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  struct StampRectangle {
    PixelCoordinate m_min_coord, m_max_coord;

    StampRectangle() {}
    StampRectangle(PixelCoordinate min_coord, PixelCoordinate max_coord) : m_min_coord(min_coord), m_max_coord(max_coord) {}

    int getWidth() const { return m_max_coord.m_x - m_min_coord.m_x; }
    int getHeight() const { return m_max_coord.m_y - m_min_coord.m_y; }
  };

  std::shared_ptr<VectorImage<SeFloat>> createWeightImage(SourceGroupInterface& group, int frame_index) const;
  std::shared_ptr<VectorImage<SeFloat>> createImageCopy(SourceGroupInterface& group, int frame_index) const;

  StampRectangle getStampRectangle(SourceGroupInterface& group, int frame_index) const;
  bool isFrameValid(SourceGroupInterface& group, int frame_index) const;
  std::tuple<double, double, double, double> computeJacobianForFrame(SourceGroupInterface& group, int frame_index) const;

  // Task configuration
  unsigned int m_max_iterations;
  std::vector<std::vector<int>> m_frame_indices_per_band;
  std::vector<std::shared_ptr<ImagePsf>> m_psfs;

  std::map<int, std::shared_ptr<VectorImage<SeFloat>>> m_debug_images;
  std::map<int, std::shared_ptr<Image<SeFloat>>> m_residual_images;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_ */
