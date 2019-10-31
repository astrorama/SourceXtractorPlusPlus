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
 * FlexibleModelFittingTask.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASK_H_

#include "ModelFitting/Models/FrameModel.h"

#include "SEImplementation/Image/ImagePsf.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Task/GroupTask.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingFrame.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingPrior.h"

namespace SourceXtractor {

class FlexibleModelFittingTask : public GroupTask {

public:
  FlexibleModelFittingTask(unsigned int max_iterations, double modified_chi_squared_scale,
      std::vector<std::shared_ptr<FlexibleModelFittingParameter>> parameters,
      std::vector<std::shared_ptr<FlexibleModelFittingFrame>> frames,
      std::vector<std::shared_ptr<FlexibleModelFittingPrior>> priors);

  virtual ~FlexibleModelFittingTask();

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

  bool isFrameValid(SourceGroupInterface& group, int frame_index) const;

  std::shared_ptr<VectorImage<SeFloat>> createImageCopy(SourceGroupInterface& group, int frame_index) const;
  std::shared_ptr<VectorImage<SeFloat>> createWeightImage(SourceGroupInterface& group, int frame_index) const;

  ModelFitting::FrameModel<ImagePsf, std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>> createFrameModel(
      SourceGroupInterface& group,
      double pixel_scale, FlexibleModelFittingParameterManager& manager, std::shared_ptr<FlexibleModelFittingFrame> frame) const;

  void createParameters() const;

  void updateCheckImages(SourceGroupInterface& group,
      double pixel_scale, FlexibleModelFittingParameterManager& manager) const;

  SeFloat computeChiSquaredForFrame(std::shared_ptr<const Image<SeFloat>> image,
      std::shared_ptr<const Image<SeFloat>> model, std::shared_ptr<const Image<SeFloat>> weights, int& data_points) const;
  SeFloat computeChiSquared(SourceGroupInterface& group,
      double pixel_scale, FlexibleModelFittingParameterManager& manager, int& total_data_points) const;

  // Task configuration
  unsigned int m_max_iterations;
  double m_modified_chi_squared_scale;

  std::vector<std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  std::vector<std::shared_ptr<FlexibleModelFittingFrame>> m_frames;
  std::vector<std::shared_ptr<FlexibleModelFittingPrior>> m_priors;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASK_H_ */
