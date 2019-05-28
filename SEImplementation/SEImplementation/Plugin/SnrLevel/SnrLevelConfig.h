/*
 * SnrLevelConfig.h
 *
 *  Created on: May 28, 2019
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SNRLEVEL_SNRCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_SNRLEVEL_SNRCONFIG_H_

#include <Configuration/Configuration.h>

namespace SExtractor {

  class SnrLevelConfig: public Euclid::Configuration::Configuration {
public:
  virtual ~SnrLevelConfig() = default;

  SnrLevelConfig(long manager_id): Configuration(manager_id) {}

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  //void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  const double& getSnrLevel() const;

private:
  double m_snr_level;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_SNRLEVEL_SNRCONFIG_H_
