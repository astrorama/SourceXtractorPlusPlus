/** Copyright © 2019-2024 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include <boost/thread.hpp>

#include "SEImplementation/Configuration/RngConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string RNG_SEED {"rng-seed"};

RngConfig::RngConfig(long manager_id) : Configuration(manager_id), m_seed(42) {}

auto RngConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Random number generation", {
      {RNG_SEED.c_str(), po::value<unsigned int>()->default_value(42), "Random number generator seed"},
  }}};
}

void RngConfig::initialize(const UserValues& args) {
  m_seed = args.at(RNG_SEED).as<unsigned int>();
}

} // SourceXtractor namespace

