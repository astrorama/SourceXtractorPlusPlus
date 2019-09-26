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
 * @file BackgroundConfig.cpp
 * @author nikoapos
 */

#include "SEFramework/Image/SubtractImage.h"
#include "SEImplementation/Configuration/SE2BackgroundConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string CELLSIZE_VALUE {"background-cell-size" };
static const std::string SMOOTHINGBOX_VALUE {"smoothing-box-size" };

SE2BackgroundConfig::SE2BackgroundConfig(long manager_id) :
    Configuration(manager_id),
    m_cell_size(),
    m_smoothing_box() {
}

std::map<std::string, Configuration::OptionDescriptionList> SE2BackgroundConfig::getProgramOptions() {
  return { {"Background modelling", {
      {CELLSIZE_VALUE.c_str(), po::value<std::string>()->default_value(std::string("64")),
          "Background mesh cell size to determine a value."},
      {SMOOTHINGBOX_VALUE.c_str(), po::value<std::string>()->default_value(std::string("3")),
          "Background median filter size"},
  }}};
}

void SE2BackgroundConfig::initialize(const UserValues& args) {
  if (args.find(CELLSIZE_VALUE) != args.end()) {
    m_cell_size = args.find(CELLSIZE_VALUE)->second.as<std::string>();
  }
  if (args.find(SMOOTHINGBOX_VALUE) != args.end()) {
    m_smoothing_box = args.find(SMOOTHINGBOX_VALUE)->second.as<std::string>();
  }
}

} // SExtractor namespace
