/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#include <boost/program_options.hpp>
#include "ElementsKernel/Exception.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltConfig.h"
#include "SEUtils/IsNan.h"

namespace SourceXtractor {
namespace {
const std::string POSITION_PRIOR_SIGMA = "spread-model-alt-position-prior-sigma";
}

std::map<std::string, SpreadModelAltConfig::OptionDescriptionList> SpreadModelAltConfig::getProgramOptions() {
  return {{"SpreadModelAlt", {{POSITION_PRIOR_SIGMA.c_str(),
      boost::program_options::value<double>()->default_value(0.2),
      "Per-axis Gaussian frame-offset prior sigma in reference pixels (0 fixes offsets to zero)"}}}};
}

void SpreadModelAltConfig::initialize(const UserValues& args) {
  const double sigma = args.at(POSITION_PRIOR_SIGMA).as<double>();
  if (fastmath_isnan(sigma) || fastmath_isinf(sigma) || sigma < 0 ||
      (sigma > 0 && fastmath_isinf(1 / sigma))) {
    throw Elements::Exception() << POSITION_PRIOR_SIGMA << " must be finite and nonnegative with finite inverse";
  }
  m_position_prior_sigma = sigma;
}

} // namespace SourceXtractor
