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
 * MagnitudeConfig.cpp
 *
 *  Created on: Sep 29, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/MagnitudeConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string MAG_ZEROPOINT {"magnitude-zero-point"};

MagnitudeConfig::MagnitudeConfig(long manager_id) :
    Configuration(manager_id),
    m_magnitude_zero_point(0) {
}

auto MagnitudeConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Magnitude", {
      {MAG_ZEROPOINT.c_str(), po::value<double>()->default_value(0.0), "Magnitude zero point calibration"}
  }}};
}

void MagnitudeConfig::initialize(const UserValues& args) {
  if (args.count(MAG_ZEROPOINT) != 0) {
    m_magnitude_zero_point = SeFloat(args.at(MAG_ZEROPOINT).as<double>());
  }
}

}

