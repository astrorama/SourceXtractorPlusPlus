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

static const std::string SAMPLING_METHOD {"sampling-method"};
static const std::string SAMPLING_SAMPLES_NB {"sampling-sample-nb"};
static const std::string SAMPLING_ADAPTIVE_TARGET {"sampling-adaptive-target"};
static const std::string SAMPLING_SCALE_FACTOR {"sampling-scale-factor"};
static const std::string SAMPLING_RENORMALIZE {"sampling-renormalize"};

static std::map<std::string, SamplingConfig::SamplingMethod> sampling_method_map {
  {"LEGACY", SamplingConfig::SamplingMethod::LEGACY},
  {"ADAPTIVE", SamplingConfig::SamplingMethod::ADAPTIVE},
  {"GRID", SamplingConfig::SamplingMethod::GRID},
  {"STOCHASTIC", SamplingConfig::SamplingMethod::STOCHASTIC},
};

SamplingConfig::SamplingConfig(long manager_id) : Configuration(manager_id),
    m_sampling_method(SamplingMethod::ADAPTIVE), m_sample_nb(8), m_adaptive_target(0.001), m_renormalize(true) {}

auto SamplingConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Model Fitting Sampling",
      {
        {SAMPLING_METHOD.c_str(), po::value<std::string>()->default_value("ADAPTIVE"),
         "Sampling method used for model fitting (LEGACY, ADAPTIVE, GRID or STOCHASTIC)"},
        {SAMPLING_SAMPLES_NB.c_str(), po::value<int>()->default_value(64),
         "Number of samples (for adpative/grid size of grid, for stochastic total)"},
        {SAMPLING_ADAPTIVE_TARGET.c_str(), po::value<double>()->default_value(0.001),
         "Adaptive sampling will stop when difference is less than this"},
        {SAMPLING_SCALE_FACTOR.c_str(), po::value<double>()->default_value(1.0),
         "Scaling factor for the rendering of models (e.g. 2 = twice the resolution)"},
        {SAMPLING_RENORMALIZE.c_str(), po::value<bool>()->default_value(true),
         "Renormalize flux in the rendered models"}
      }
  }};
}

void SamplingConfig::preInitialize(const UserValues& args) {
  auto method_name = boost::to_upper_copy(args.at(SAMPLING_METHOD).as<std::string>());
  if (sampling_method_map.count(method_name) == 0) {
    throw Elements::Exception() << "Unknown sampling method: " << method_name;
  }
}

void SamplingConfig::initialize(const UserValues& args) {
  m_sample_nb = args.at(SAMPLING_SAMPLES_NB).as<int>();
  m_adaptive_target = args.at(SAMPLING_ADAPTIVE_TARGET).as<double>();
  m_scale_factor = args.at(SAMPLING_SCALE_FACTOR).as<double>();

  auto method_name  = boost::to_upper_copy(args.at(SAMPLING_METHOD).as<std::string>());
  m_sampling_method = sampling_method_map.at(method_name);
  m_renormalize = args.at(SAMPLING_RENORMALIZE).as<bool>();
}

} /* namespace SourceXtractor */
