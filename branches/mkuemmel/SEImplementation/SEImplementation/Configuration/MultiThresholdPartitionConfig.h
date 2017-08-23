/*
 * MultiThresholdPartitionConfig.h
 *
 *  Created on: Jan 18, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MULTITHRESHOLDPARTITIONCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MULTITHRESHOLDPARTITIONCONFIG_H_


#include "Configuration/Configuration.h"

namespace SExtractor {

class MultiThresholdPartitionConfig : public Euclid::Configuration::Configuration {

public:

  MultiThresholdPartitionConfig(long manager_id);

  virtual ~MultiThresholdPartitionConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

};

} /* namespace SExtractor */


#endif /* _SEIMPLEMENTATION_CONFIGURATION_MULTITHRESHOLDPARTITIONCONFIG_H_ */
