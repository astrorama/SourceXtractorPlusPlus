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
 * ModelFittingConfig.h
 *
 *  Created on: May 17, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_LEGACYMODELFITTINGCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_LEGACYMODELFITTINGCONFIG_H_

#include "Configuration/Configuration.h"

namespace SourceXtractor {

class LegacyModelFittingConfig : public Euclid::Configuration::Configuration {

public:

  LegacyModelFittingConfig(long manager_id);

  virtual ~LegacyModelFittingConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  int getMaxIterations() const {
    return m_max_iterations;
  }

  std::string getLeastSquaresEngine() const {
    return m_least_squares_engine;
  }

private:
  std::string m_least_squares_engine;
  int m_max_iterations;

};

} /* namespace SourceXtractor */




#endif /* _SEIMPLEMENTATION_CONFIGURATION_LEGACYMODELFITTINGCONFIG_H_ */
