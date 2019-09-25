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
 * CleaningConfig.cpp
 *
 *  Created on: 2019 M02 6
 *      Author: mschefer
 */

#include <SEImplementation/Configuration/DeblendStepConfig.h>
#include "SEFramework/Pipeline/Deblending.h"

#include "SEImplementation/Deblending/Cleaning.h"

#include "SEImplementation/Configuration/CleaningConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string USE_CLEANING {"use-cleaning"};
static const std::string CLEANING_MINAREA {"cleaning-minimum-area"};

CleaningConfig::CleaningConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<DeblendStepConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> CleaningConfig::getProgramOptions() {
  return { {"Cleaning", {
      {USE_CLEANING.c_str(), po::bool_switch(),
         "Enables the cleaning of sources (removes false detections near bright objects)"},
      {CLEANING_MINAREA.c_str(), po::value<int>()->default_value(3), "min. # of pixels above threshold"}
  }}};
}

void CleaningConfig::initialize(const UserValues& args) {
  auto min_area = args.at(CLEANING_MINAREA).as<int>();
  if (args.at(USE_CLEANING).as<bool>()) {
    if (min_area <= 0) {
        throw Elements::Exception() << "Invalid " << CLEANING_MINAREA << " value: " << min_area;
    }
    getDependency<DeblendStepConfig>().addDeblendStepCreator(
        [min_area](std::shared_ptr<SourceFactory> source_factory) {
          return std::make_shared<Cleaning>(source_factory, min_area);
        }
    );
  }
}

} // SExtractor namespace



