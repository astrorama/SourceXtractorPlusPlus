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

#include "SEImplementation/Plugin/FluxRadius/FluxRadiusConfig.h"
#include <AlexandriaKernel/StringUtils.h>

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string FLUX_FRACTION{"flux-fraction"};

FluxRadiusConfig::FluxRadiusConfig(long managerId) : Configuration(managerId) {}

auto FluxRadiusConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Flux radius", {
    {FLUX_FRACTION.c_str(), po::value<std::string>()->default_value("0.5"),
      "Fraction of flux defining each element of the flux_radius vector"}
  }}};
}

void FluxRadiusConfig::initialize(const Configuration::UserValues& args) {
  auto flux_fraction_str = args.at(FLUX_FRACTION).as<std::string>();
  m_flux_fraction = Euclid::stringToVector<SeFloat>(flux_fraction_str);
  if (m_flux_fraction.empty()) {
    throw Elements::Exception() << FLUX_FRACTION << " must contain at least one element";
  }
  for (auto frac : m_flux_fraction) {
    if (frac <= 0 || frac >= 1) {
      throw Elements::Exception() << FLUX_FRACTION << " only accepts values strictly between 0 and 1";
    }
  }
}

const std::vector<SeFloat>& FluxRadiusConfig::getFluxFraction() const {
  return m_flux_fraction;
}

} // end of namespace SourceXtractor
