/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * PsfPluginConfig.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro ��lvarez Ayll��n
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSFCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSFCONFIG_H_

#include <Configuration/Configuration.h>
#include <SEFramework/Psf/VariablePsf.h>
#include <SEFramework/Psf/VariablePsfStack.h>
#include "SEImplementation/Image/ImagePsf.h"

namespace SourceXtractor {

class PsfPluginConfig: public Euclid::Configuration::Configuration {
public:
  virtual ~PsfPluginConfig() = default;

  PsfPluginConfig(long manager_id): Configuration(manager_id) {}

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  const std::shared_ptr<VariablePsf>& getPsf() const;

  static std::shared_ptr<VariablePsf> readPsf(const std::string &filename, int hdu_number = 1);
  static std::shared_ptr<VariablePsf> generateGaussianPsf(SeFloat fwhm, SeFloat pixel_sampling);

private:
  std::shared_ptr<VariablePsf> m_vpsf;
};

} // end SourceXtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSFCONFIG_H_
