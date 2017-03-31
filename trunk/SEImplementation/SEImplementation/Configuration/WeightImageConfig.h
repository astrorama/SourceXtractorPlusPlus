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

  enum class WeightType {
    WEIGHT_TYPE_NONE,
    WEIGHT_TYPE_FROM_BACKGROUND,
    WEIGHT_TYPE_ABS_RMS,
    WEIGHT_TYPE_REL_VARIANCE,
    WEIGHT_TYPE_REL_WEIGHT,
  };

  virtual ~WeightImageConfig() = default;
  WeightImageConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

  std::shared_ptr<WeightImage> getWeightImage() const {
    return m_weight_image;
  }

  WeightType getWeightType() const {
    return m_weight_type;
  }

  bool isWeightAbsolute() const {
    return m_weight_type != WeightType::WEIGHT_TYPE_REL_VARIANCE && m_weight_type != WeightType::WEIGHT_TYPE_REL_WEIGHT;
  }



private:
  void convertWeightMap(WeightType weight_type);

  std::shared_ptr<WeightImage> m_weight_image;
  WeightType m_weight_type;
};


}


#endif /* _SEIMPLEMENTATION_CONFIGURATION_WEIGHTIMAGECONFIG_H_ */
