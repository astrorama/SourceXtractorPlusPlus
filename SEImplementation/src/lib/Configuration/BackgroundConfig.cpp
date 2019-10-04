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

#include "SEFramework/Image/ProcessedImage.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string BACKGROUND_VALUE {"background-value" };
static const std::string THRESHOLD_VALUE {"detection-threshold" };

BackgroundConfig::BackgroundConfig(long manager_id) :
    Configuration(manager_id),
    m_background_level_absolute(false),
    m_background_level(0),
    m_detection_threshold_absolute(false),
    m_detection_threshold(0) {
}

std::map<std::string, Configuration::OptionDescriptionList> BackgroundConfig::getProgramOptions() {
  return { {"Detection image", {
      {BACKGROUND_VALUE.c_str(), po::value<double>(),
          "Background value to be subtracted from the detection image."},
      {THRESHOLD_VALUE.c_str(), po::value<double>()->default_value((1.5)),
          "Detection threshold above the background."},
  }}};
}

void BackgroundConfig::initialize(const UserValues& args) {
  if (args.find(BACKGROUND_VALUE) != args.end()) {
    m_background_level_absolute = true;
    m_background_level = args.find(BACKGROUND_VALUE)->second.as<double>();
  }
  if (args.find(THRESHOLD_VALUE) != args.end()) {
    m_detection_threshold_absolute = true;
    m_detection_threshold = args.find(THRESHOLD_VALUE)->second.as<double>();
  }
}

} // SExtractor namespace
