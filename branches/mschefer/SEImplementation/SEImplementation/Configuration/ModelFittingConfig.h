/*
 * ModelFittingConfig.h
 *
 *  Created on: May 17, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H_

#include "Configuration/Configuration.h"

namespace SExtractor {

class ModelFittingConfig : public Euclid::Configuration::Configuration {

public:

  ModelFittingConfig(long manager_id);

  virtual ~ModelFittingConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  unsigned int getMaxIterations() const {
    return m_max_iterations;
  }

private:
  unsigned int m_max_iterations;

};

} /* namespace SExtractor */




#endif /* _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H_ */
