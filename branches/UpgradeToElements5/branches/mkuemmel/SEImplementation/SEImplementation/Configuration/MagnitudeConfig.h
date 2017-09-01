/*
 * MagnitudeConfig.h
 *
 *  Created on: Sep 29, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MAGNITUDECONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MAGNITUDECONFIG_H_

#include "Configuration/Configuration.h"
#include "SEUtils/Types.h"

namespace SExtractor {

class MagnitudeConfig : public Euclid::Configuration::Configuration {

public:

  MagnitudeConfig(long manager_id);

  virtual ~MagnitudeConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  SeFloat getMagnitudeZeroPoint() const {
    return m_magnitude_zero_point;
  }

private:
  SeFloat m_magnitude_zero_point;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_CONFIGURATION_MAGNITUDECONFIG_H_ */
