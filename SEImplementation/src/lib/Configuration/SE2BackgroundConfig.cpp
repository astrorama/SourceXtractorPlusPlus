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

#include <algorithm>
#include <ElementsKernel/Logging.h>
#include "AlexandriaKernel/StringUtils.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEImplementation/Configuration/SE2BackgroundConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static Elements::Logging logger = Elements::Logging::getLogger("BackgroundConfig");

static const std::string CELLSIZE_VALUE {"background-cell-size" };
static const std::string SMOOTHINGBOX_VALUE {"smoothing-box-size" };
static const std::string LEGACY_BACKGROUND {"background-legacy"};

SE2BackgroundConfig::SE2BackgroundConfig(long manager_id) :
  Configuration(manager_id), m_cell_size(), m_smoothing_box() {
}

std::map<std::string, Configuration::OptionDescriptionList> SE2BackgroundConfig::getProgramOptions() {
  return { {"Background modelling", {
      {CELLSIZE_VALUE.c_str(), po::value<std::string>()->default_value(std::string("64")),
          "Background mesh cell size to determine a value."},
      {SMOOTHINGBOX_VALUE.c_str(), po::value<std::string>()->default_value(std::string("3")),
          "Background median filter size"},
      {LEGACY_BACKGROUND.c_str(), po::bool_switch(),
          "Deprecated, kept for compatibility"}
  }}};
}

void SE2BackgroundConfig::initialize(const UserValues& args) {
  auto cell_size_str = args.find(CELLSIZE_VALUE)->second.as<std::string>();
  auto smoothing_box_str = args.find(SMOOTHINGBOX_VALUE)->second.as<std::string>();

  if (args.find(CELLSIZE_VALUE) != args.end()) {
    m_cell_size = Euclid::stringToVector<int>(cell_size_str);
  }
  if (args.find(SMOOTHINGBOX_VALUE) != args.end()) {
    m_smoothing_box = Euclid::stringToVector<int>(smoothing_box_str);
  }

  auto less_eq_0 = [](int v) { return v <= 0; };
  auto less_0 = [](int v) { return v < 0; };

  if (std::find_if(m_cell_size.begin(), m_cell_size.end(), less_eq_0) != m_cell_size.end()) {
    throw Elements::Exception() << "There are value(s) < 1 in backgound-cell-size: " << cell_size_str;
  }
  if (std::find_if(m_smoothing_box.begin(), m_smoothing_box.end(), less_0) != m_smoothing_box.end()) {
    throw Elements::Exception() << "There are value(s) < 0 in smoothing-box-size: " << smoothing_box_str;
  }
  if (args.find(LEGACY_BACKGROUND) != args.end()) {
    logger.warn() << "The option "
                  << LEGACY_BACKGROUND << " is deprecated and has no effect starting at version 0.17";
  }
}

} // SourceXtractor namespace
