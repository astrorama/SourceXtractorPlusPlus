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
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LeastSquareEngineManager.h"

#include "SEUtils/PixelRectangle.h"

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
      double scale_factor=1.0,
      int meta_iterations=3,
      double deblend_factor=1.0,
      double meta_iteration_stop=0.0001,
      size_t max_fit_size=100
      );

  virtual ~FlexibleModelFittingIterativeTask();

  void computeProperties(SourceGroupInterface& group) const override;

private:
  struct SourceState {
    std::unordered_map<int, double> parameters_values;
    std::unordered_map<int, double> parameters_sigmas;
    std::unordered_map<int, bool> parameters_fitted;
    Flags flags;
    double reduced_chi_squared;
    float duration;
    unsigned int iterations;
    unsigned int stop_reason;
    std::vector<SeFloat> chi_squared_per_meta;
    std::vector<int> iterations_per_meta;
  };

  struct FittingState {
    std::vector<SourceState> source_states;
  };

  std::shared_ptr<VectorImage<SeFloat>> createDeblendImage(
      SourceGroupInterface& group, SourceInterface& source, int source_index,
      std::shared_ptr<FlexibleModelFittingFrame> frame, FittingState& state) const;

  void fitSource(SourceGroupInterface& group, SourceInterface& source, int index, FittingState& state) const;
  void updateCheckImages(SourceGroupInterface& group, double pixel_scale, FittingState& state) const;
  SeFloat computeChiSquared(SourceGroupInterface& group, SourceInterface& source, int index,
      double pixel_scale, FlexibleModelFittingParameterManager& manager, int& total_data_points, FittingState& state) const;
  SeFloat computeChiSquaredForFrame(std::shared_ptr<const Image<SeFloat>> image,
      std::shared_ptr<const Image<SeFloat>> model, std::shared_ptr<const Image<SeFloat>> weights, int& data_points) const;
  int fitSourcePrepareParameters(FlexibleModelFittingParameterManager& parameter_manager,
                                 ModelFitting::EngineParameterManager& engine_parameter_manager,
                                 SourceInterface& source, int index, FittingState& state) const;
  int fitSourcePrepareModels(FlexibleModelFittingParameterManager& parameter_manager,
      ModelFitting::ResidualEstimator& res_estimator, int& good_pixels,
      SourceGroupInterface& group, SourceInterface& source, int index, FittingState& state, double downscaling) const;
  SeFloat fitSourceComputeChiSquared(FlexibleModelFittingParameterManager& parameter_manager,
      SourceGroupInterface& group, SourceInterface& source, int index, FittingState& state) const;
  void fitSourceUpdateState(FlexibleModelFittingParameterManager& parameter_manager, SourceInterface& source,
      SeFloat avg_reduced_chi_squared, SeFloat duration, unsigned int iterations, unsigned int stop_reason, Flags flags,
      ModelFitting::LeastSquareSummary solution,
      int index, FittingState& state) const;

  // Task configuration
  std::string m_least_squares_engine;
  unsigned int m_max_iterations;
  double m_modified_chi_squared_scale;
  double m_scale_factor;
  int m_meta_iterations;
  double m_deblend_factor;
  double m_meta_iteration_stop;
  size_t m_max_fit_size;

  std::vector<std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  std::vector<std::shared_ptr<FlexibleModelFittingFrame>> m_frames;
  std::vector<std::shared_ptr<FlexibleModelFittingPrior>> m_priors;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGITERATIVETASK_H_ */
