/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

  // Task configuration
  unsigned int m_max_iterations;
  std::vector<std::vector<int>> m_frame_indices_per_band;

  std::map<int, std::shared_ptr<VectorImage<SeFloat>>> m_debug_images;
  std::map<int, std::shared_ptr<Image<SeFloat>>> m_residual_images;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMEMODELFITTINGTASK_H_ */
