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
 * CoreThresholdPartitionConfig.h
 *
 *  Created on: May 28, 2019
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDCONFIG_H_

#include "Configuration/Configuration.h"

namespace SourceXtractor {

  class CoreThresholdPartitionConfig: public Euclid::Configuration::Configuration {
public:
  virtual ~CoreThresholdPartitionConfig() = default;

  explicit CoreThresholdPartitionConfig(long manager_id);

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  //void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  const double& getCoreThreshold() const;

  const int& getCoreMinArea() const;

private:
  double m_core_threshold;
  int    m_core_minarea;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_SNRLEVEL_SNRCONFIG_H_
