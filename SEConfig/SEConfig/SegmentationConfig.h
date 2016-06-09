/**
 * @file SEConfig/SegmentationConfig.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SECONFIG_SEGMENTATIONCONFIG_H
#define _SECONFIG_SEGMENTATIONCONFIG_H

#include "Configuration/Configuration.h"

namespace SEConfig {

/**
 * @class SegmentationConfig
 * @brief
 *
 */
class SegmentationConfig : public Euclid::Configuration::Configuration {
public:

  /**
   * @brief Destructor
   */
  virtual ~SegmentationConfig() = default;

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

private:

}; /* End of SegmentationConfig class */

} /* namespace SEConfig */


#endif
