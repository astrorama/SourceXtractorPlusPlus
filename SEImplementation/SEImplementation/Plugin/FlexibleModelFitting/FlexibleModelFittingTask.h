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

namespace SExtractor {

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
  std::tuple<double, double, double, double> computeJacobianForFrame(
                                                            SourceGroupInterface& group, int frame_index) const;
  std::shared_ptr<VectorImage<SeFloat>> createImageCopy(SourceGroupInterface& group, int frame_index) const;
  std::shared_ptr<VectorImage<SeFloat>> createWeightImage(SourceGroupInterface& group, int frame_index) const;

  ModelFitting::FrameModel<ImagePsf, std::shared_ptr<VectorImage<SExtractor::SeFloat>>> createFrameModel(
      SourceGroupInterface& group,
      double pixel_scale, FlexibleModelFittingParameterManager& manager, std::shared_ptr<FlexibleModelFittingFrame> frame) const;

  void createParameters() const;

  void updateCheckImages(SourceGroupInterface& group,
      double pixel_scale, FlexibleModelFittingParameterManager& manager) const;

  SeFloat computeReducedChiSquaredForFrame(std::shared_ptr<const Image<SeFloat>> image,
      std::shared_ptr<const Image<SeFloat>> model, std::shared_ptr<const Image<SeFloat>> weights,
      int nb_of_free_params) const;
  SeFloat computeReducedChiSquared(SourceGroupInterface& group,
      double pixel_scale, FlexibleModelFittingParameterManager& manager) const;

  // Task configuration
  unsigned int m_max_iterations;
  double m_modified_chi_squared_scale;

  std::vector<std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  std::vector<std::shared_ptr<FlexibleModelFittingFrame>> m_frames;
  std::vector<std::shared_ptr<FlexibleModelFittingPrior>> m_priors;

  std::string m_checkimage_prefix;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGTASK_H_ */
