/*
 * AutoPhotometryConfig.h
 *
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYCONFIG_H_

#include <vector>
#include "Configuration/Configuration.h"

namespace SExtractor {

class AutoPhotometryConfig : public Euclid::Configuration::Configuration {

public:
  /// Destructor
  virtual ~AutoPhotometryConfig() = default;

  AutoPhotometryConfig(long manager_id) : Configuration(manager_id) {}

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  //void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  SeFloat getAutoKronFactor() const;
  SeFloat getAutoKronMinrad() const;
  SeFloat getAutoKronEstimation() const;
  SeFloat getAutoKronMeasurement() const;

private:
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  SeFloat m_kron_estimation;
  SeFloat m_kron_measurement;
};

} /* namespace SExtractor */


#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYCONFIG_H_ */
