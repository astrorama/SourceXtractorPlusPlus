/*
 * WeightImageConfig.h
 *
 *  Created on: Oct 7, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_WEIGHTIMAGECONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_WEIGHTIMAGECONFIG_H_

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class WeightImageConfig : public Euclid::Configuration::Configuration {

public:

  virtual ~WeightImageConfig() = default;
  WeightImageConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

  std::shared_ptr<WeightImage> getWeightImage() const {
    return m_weight_image;
  }

private:
  std::shared_ptr<WeightImage> m_weight_image;

};


}


#endif /* _SEIMPLEMENTATION_CONFIGURATION_WEIGHTIMAGECONFIG_H_ */
