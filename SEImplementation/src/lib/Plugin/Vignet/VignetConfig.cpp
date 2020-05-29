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
 * @file VignetConfig.cpp
 * @author mkuemmel
 */

#include <AlexandriaKernel/StringUtils.h>
#include "SEFramework/Image/ProcessedImage.h"
#include "SEImplementation/Plugin/Vignet/VignetConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string VIGNET_SIZE {"vignet-size" };
static const std::string VIGNET_DEFAULT_PIXVAL {"vignet-default-pixval" };

VignetConfig::VignetConfig(long manager_id) :
  Configuration(manager_id),
  m_vignet_size(),
  m_vignet_default_pixval() {
}

std::map<std::string, Configuration::OptionDescriptionList> VignetConfig::getProgramOptions() {
  return { {"Vignet output", {
      {VIGNET_SIZE.c_str(), po::value<std::string>()->default_value(std::string("15,15")),
          "X- and Y-size of the vignet."},
	      {VIGNET_DEFAULT_PIXVAL.c_str(), po::value<double>()->default_value(std::nan("")), //Note: the SE2 value is "1.0E30", but nan is more consistent
          "Default pixel value for the vignet data"},
  }}};
}

void VignetConfig::initialize(const UserValues& args) {
  auto vignet_iter = args.find(VIGNET_SIZE);
  if (vignet_iter != args.end()) {
    auto vignet_str = vignet_iter->second.as<std::string>();
    auto vignet_vector = Euclid::stringToVector<int>(vignet_str);
    if (vignet_vector.size() > 2) {
      throw Elements::Exception() << VIGNET_SIZE << " only accepts one or two numbers";
    }
    m_vignet_size[0] = vignet_vector.front();
    m_vignet_size[1] = vignet_vector.back();
    if (m_vignet_size[0] < 0 || m_vignet_size[1] < 0) {
      throw Elements::Exception() << VIGNET_SIZE << " only accept positive numbers";
    }
  }
  if (args.find(VIGNET_DEFAULT_PIXVAL) != args.end()) {
    m_vignet_default_pixval = args.find(VIGNET_DEFAULT_PIXVAL)->second.as<double>();
  }
}

} // SourceXtractor namespace
