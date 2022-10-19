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

#include <boost/program_options.hpp>
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveConfig.h"

namespace po = boost::program_options;
using namespace Euclid::Configuration;

namespace SourceXtractor {

const static std::string GROWTH_NSAMPLES{"flux-growth-samples"};

GrowthCurveConfig::GrowthCurveConfig(long managerId) : Configuration(managerId) {}

auto GrowthCurveConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Growth curve", {
    {GROWTH_NSAMPLES.c_str(), po::value<int>()->default_value(64), "Number of samples to take from the growth curve"}
  }}};
}

void GrowthCurveConfig::initialize(const Configuration::UserValues& args) {
  if (args.count(GROWTH_NSAMPLES)) {
    m_nsamples = args.at(GROWTH_NSAMPLES).as<int>();
    if (m_nsamples <= 0) {
      throw Elements::Exception() << GROWTH_NSAMPLES << " must be greater than 0";
    }
  }
}

} // end of namespace SourceXtractor
