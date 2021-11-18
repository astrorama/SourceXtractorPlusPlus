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

#include <boost/algorithm/string.hpp>

#include "SEImplementation/Configuration/SamplingConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

//static const std::string SAMPLING_ADAPTIVE_TARGET {"sampling-adaptive-target"};
static const std::string SAMPLING_SCALE_FACTOR {"sampling-scale-factor"};
static const std::string SAMPLING_MAX_FIT_SIZE {"sampling-max-fit-size"};

SamplingConfig::SamplingConfig(long manager_id) : Configuration(manager_id),
    m_adaptive_target(0.001), m_scale_factor(1.0), m_max_fit_size(1000) {}

auto SamplingConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Model Fitting Sampling",
      {
//        {SAMPLING_ADAPTIVE_TARGET.c_str(), po::value<double>()->default_value(0.001),
//         "Adaptive sampling will stop when difference is less than this"},
        {SAMPLING_SCALE_FACTOR.c_str(), po::value<double>()->default_value(1.0),
         "Scaling factor for the rendering of models (e.g. 2 = twice the resolution)"},
        {SAMPLING_MAX_FIT_SIZE.c_str(), po::value<size_t>()->default_value(1000),
         "Size of maximum fit area before downsampling (in pixels, one side)"}
      }
  }};
}

void SamplingConfig::preInitialize(const UserValues& args) {
}

void SamplingConfig::initialize(const UserValues& args) {
//  m_adaptive_target = args.at(SAMPLING_ADAPTIVE_TARGET).as<double>();
  m_scale_factor = args.at(SAMPLING_SCALE_FACTOR).as<double>();
  m_max_fit_size = args.at(SAMPLING_MAX_FIT_SIZE).as<size_t>();
}

} /* namespace SourceXtractor */
