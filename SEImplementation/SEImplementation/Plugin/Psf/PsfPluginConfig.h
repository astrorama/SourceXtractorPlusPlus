/*
 * PsfPluginConfig.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSFCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSFCONFIG_H_

#include <Configuration/Configuration.h>
#include <SEFramework/Psf/VariablePsf.h>
#include "SEImplementation/Image/ImagePsf.h"

namespace SExtractor {

class PsfPluginConfig: public Euclid::Configuration::Configuration {
public:
  virtual ~PsfPluginConfig() = default;

  PsfPluginConfig(long manager_id): Configuration(manager_id) {}

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  const std::shared_ptr<VariablePsf>& getPsf() const;

  static std::shared_ptr<VariablePsf> readPsf(const std::string &filename);
  static std::shared_ptr<VariablePsf> generateGaussianPsf(SeFloat fwhm, SeFloat pixel_scale);

private:
  std::shared_ptr<VariablePsf> m_vpsf;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSFCONFIG_H_
