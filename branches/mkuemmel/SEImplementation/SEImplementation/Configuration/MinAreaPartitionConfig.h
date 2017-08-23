/* 
 * @file MinAreaPartitionConfig.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_MINAREAPARTITIONCONFIG_H
#define _SEIMPLEMENTATION_MINAREAPARTITIONCONFIG_H

#include "Configuration/Configuration.h"

namespace SExtractor {

class MinAreaPartitionConfig : public Euclid::Configuration::Configuration {
  
public:
  
  MinAreaPartitionConfig(long manager_id);
  
  virtual ~MinAreaPartitionConfig() = default;
  
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_MINAREAPARTITIONCONFIG_H */

