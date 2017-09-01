/*
 * CheckImagesConfig.h
 *
 *  Created on: May 29, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_CHECKIMAGESCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_CHECKIMAGESCONFIG_H_

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class CheckImagesConfig : public Euclid::Configuration::Configuration {

public:

  CheckImagesConfig(long manager_id);

  virtual ~CheckImagesConfig() = default;

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  const std::string& getModelFittingImageFilename() const {
    return m_model_fitting_filename;
  }

  const std::string& getModelFittingResidualFilename() const {
    return m_model_fitting_residual_filename;
  }

private:

  std::string m_model_fitting_filename;
  std::string m_model_fitting_residual_filename;


};

}



#endif /* _SEIMPLEMENTATION_CONFIGURATION_CheckImagesSCONFIG_H_ */
