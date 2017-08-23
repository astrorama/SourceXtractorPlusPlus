/* 
 * @file AttractorsPartitionConfig.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_ATTRACTORSPARTITIONCONFIG_H
#define _SEIMPLEMENTATION_ATTRACTORSPARTITIONCONFIG_H

#include "Configuration/Configuration.h"

namespace SExtractor {

class AttractorsPartitionConfig : public Euclid::Configuration::Configuration {
  
public:
  
  AttractorsPartitionConfig(long manager_id);
  
  virtual ~AttractorsPartitionConfig() = default;
  
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_ATTRACTORSPARTITIONCONFIG_H */

