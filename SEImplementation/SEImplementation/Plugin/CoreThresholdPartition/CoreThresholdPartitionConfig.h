/*
 * SnrLevelConfig.h
 *
 *  Created on: May 28, 2019
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDCONFIG_H_

#include "Configuration/Configuration.h"

namespace SExtractor {

  class CoreThresholdPartitionConfig: public Euclid::Configuration::Configuration {
public:
  virtual ~CoreThresholdPartitionConfig() = default;

  CoreThresholdPartitionConfig(long manager_id);

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  //void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  const double& getCoreThreshold() const;

  const int& getCoreMinArea() const;

private:
  double m_core_threshold;
  int    m_core_minarea;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_SNRLEVEL_SNRCONFIG_H_
