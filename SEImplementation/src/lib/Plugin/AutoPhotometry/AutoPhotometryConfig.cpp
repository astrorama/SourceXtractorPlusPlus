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
 * AperturePhotometryConfig.cpp
 *
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#include "SEUtils/Types.h"

#include "Configuration/ProgramOptionsHelper.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryConfig.h"

namespace po = boost::program_options;
using poh = Euclid::Configuration::ProgramOptionsHelper;

namespace SExtractor {

namespace {
const std::string AUTO_KRON_FACTOR {"auto-kron-factor"};
const std::string AUTO_KRON_MINRAD {"auto-kron-min-radius"};
}

auto AutoPhotometryConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Auto (Kron) photometry options", {
      {AUTO_KRON_FACTOR.c_str(), po::value<double>()->default_value(2.5), "Scale factor for AUTO (Kron) photometry"},
      {AUTO_KRON_MINRAD.c_str(), po::value<double>()->default_value(3.5), "Minimum radius for AUTO (Kron) photometry"},
  }}};
}

void AutoPhotometryConfig::initialize(const UserValues& args) {
  m_kron_factor = SeFloat(args.at(AUTO_KRON_FACTOR).as<double>());;
  m_kron_minrad = SeFloat(args.at(AUTO_KRON_MINRAD).as<double>());;
}

SeFloat AutoPhotometryConfig::getAutoKronFactor() const {
  return m_kron_factor;
}
SeFloat AutoPhotometryConfig::getAutoKronMinrad() const {
  return m_kron_minrad;
}

} // SExtractor namespace
