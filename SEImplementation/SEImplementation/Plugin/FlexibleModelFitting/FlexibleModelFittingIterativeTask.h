/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGITERATIVETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGITERATIVETASK_H_

#include "ModelFitting/Models/FrameModel.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Task/GroupTask.h"

//#include "SEImplementation/Configuration/SamplingConfig.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingFrame.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingPrior.h"

namespace SourceXtractor {

class FlexibleModelFittingIterativeTask : public GroupTask {

public:
  FlexibleModelFittingIterativeTask(const std::string &least_squares_engine,
      unsigned int max_iterations, double modified_chi_squared_scale,
      std::vector<std::shared_ptr<FlexibleModelFittingParameter>> parameters,
      std::vector<std::shared_ptr<FlexibleModelFittingFrame>> frames,
      std::vector<std::shared_ptr<FlexibleModelFittingPrior>> priors,
      double scale_factor=1.0
      );

  virtual ~FlexibleModelFittingIterativeTask();

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:
  void fitSource(SourceInterface& source) const;
  void setDummyProperty(SourceInterface& source, Flags flags) const;
  void updateCheckImages(
      SourceInterface& source, double pixel_scale, FlexibleModelFittingParameterManager& manager) const;

  // Task configuration
  std::string m_least_squares_engine;
  unsigned int m_max_iterations;
  double m_modified_chi_squared_scale;

  std::vector<std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  std::vector<std::shared_ptr<FlexibleModelFittingFrame>> m_frames;
  std::vector<std::shared_ptr<FlexibleModelFittingPrior>> m_priors;

  double m_scale_factor;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGITERATIVETASK_H_ */
