/* 
 * @file ModelFittingConfig.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H
#define _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H

#include <vector>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingModel.h>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingFrame.h>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingPrior.h>
#include <Configuration/Configuration.h>

namespace SExtractor {

class ModelFittingConfig : public Euclid::Configuration::Configuration {
  
public:
  
  ModelFittingConfig(long manager_id);
  
  void initialize(const UserValues& args) override;
  
  const std::map<int, std::shared_ptr<FlexibleModelFittingParameter>>& getParameters() const;
  
  const std::map<int, std::shared_ptr<FlexibleModelFittingModel>>& getModels() const;
  const std::vector<std::shared_ptr<FlexibleModelFittingFrame>>& getFrames() const;
  
  const std::map<int, std::shared_ptr<FlexibleModelFittingPrior>>& getPriors() const;
  
  const std::vector<std::pair<std::string, std::vector<int>>>& getOutputs() const;
  
  unsigned int getMaxIterations() const { return m_max_iterations; }

private:
  unsigned int m_max_iterations {0};
  
  std::map<int, std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  std::map<int, std::shared_ptr<FlexibleModelFittingModel>> m_models;
  std::vector<std::shared_ptr<FlexibleModelFittingFrame>> m_frames;
  std::map<int, std::shared_ptr<FlexibleModelFittingPrior>> m_priors;
  std::vector<std::pair<std::string, std::vector<int>>> m_outputs;
  
};

}

#endif // _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H

