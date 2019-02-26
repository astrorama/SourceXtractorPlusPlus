/*
 * CleaningConfig.h
 *
 *  Created on: 2019 M02 6
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_CLEANINGCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_CLEANINGCONFIG_H_


#include "Configuration/Configuration.h"

namespace SExtractor {

class CleaningConfig : public Euclid::Configuration::Configuration {

public:

  CleaningConfig(long manager_id);

  virtual ~CleaningConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

};

} /* namespace SExtractor */



#endif /* _SEIMPLEMENTATION_CONFIGURATION_CLEANINGCONFIG_H_ */
