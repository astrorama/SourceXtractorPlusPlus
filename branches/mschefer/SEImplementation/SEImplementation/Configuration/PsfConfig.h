/*
 * PsfConfig.h
 *
 *  Created on: May 23, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_PSFCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_PSFCONFIG_H_

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"
#include "ModelFitting/Image/OpenCvPsf.h"


namespace SExtractor {

// Provides a Psf for model fitting

class PsfConfig : public Euclid::Configuration::Configuration {

public:

  PsfConfig(long manager_id);

  virtual ~PsfConfig() = default;

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  std::shared_ptr<ModelFitting::OpenCvPsf> getPsf() const {
    return m_psf;
  }

  static std::shared_ptr<ModelFitting::OpenCvPsf> generateGaussianPsf(SeFloat fwhm, SeFloat pixel_scale);
  static std::shared_ptr<ModelFitting::OpenCvPsf> readPsf(const std::string& filename);

private:

  std::shared_ptr<ModelFitting::OpenCvPsf> m_psf;
};


}



#endif /* _SEIMPLEMENTATION_CONFIGURATION_PSFCONFIG_H_ */
