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

#ifndef _SEIMPLEMENTATION_PLUGIN_FLUXRADIUS_FLUXRADIUSCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_FLUXRADIUS_FLUXRADIUSCONFIG_H_

#include <Configuration/Configuration.h>
#include "SEUtils/Types.h"

namespace SourceXtractor {

class FluxRadiusConfig : public Euclid::Configuration::Configuration {
public:
  explicit FluxRadiusConfig(long managerId);

  virtual ~FluxRadiusConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  const std::vector<SeFloat>& getFluxFraction() const;

public:
  std::vector<SeFloat> m_flux_fraction;
};

} // end of namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_FLUXRADIUS_FLUXRADIUSCONFIG_H_ */
