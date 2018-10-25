/* 
 * @file ModelFittingConfig.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHONCONFIG_CONFIGURATION_MODELFITTINGCONFIG_H
#define _PYTHONCONFIG_CONFIGURATION_MODELFITTINGCONFIG_H

#include <vector>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h>
#include <Configuration/Configuration.h>

namespace SExtractor {

class ModelFittingConfig : public Euclid::Configuration::Configuration {
  
public:
  
  ModelFittingConfig(long manager_id);
  
  void initialize(const UserValues& args) override;
  
  const std::map<int, std::shared_ptr<FlexibleModelFittingParameter>>& getParameters() const;
  
private:
  
  std::map<int, std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  
};

}

#endif // _PYTHONCONFIG_CONFIGURATION_MODELFITTINGCONFIG_H

