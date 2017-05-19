/*
 * AperturePhotometryConfig.h
 *
 *  Created on: Sep 26, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYCONFIG_H_

#include <vector>
#include "Configuration/Configuration.h"

namespace SExtractor {

class AperturePhotometryConfig : public Euclid::Configuration::Configuration {

public:
  /// Destructor
  virtual ~AperturePhotometryConfig() = default;

  AperturePhotometryConfig(long manager_id) : Configuration(manager_id) {}

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  const std::vector<SeFloat>& getApertures() const;

private:
  std::vector<SeFloat> m_apertures;
};

} /* namespace SExtractor */


#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYCONFIG_H_ */
