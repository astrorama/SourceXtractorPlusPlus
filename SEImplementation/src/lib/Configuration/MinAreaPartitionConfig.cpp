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
 * @file MinAreaPartitionConfig.cpp
 * @author nikoapos
 */

#include "SEImplementation/Configuration/MinAreaPartitionConfig.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"

#include "SEImplementation/Partition/MinAreaPartitionStep.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string DETECT_MINAREA {"detection-minimum-area"};

MinAreaPartitionConfig::MinAreaPartitionConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PartitionStepConfig>();
}

auto MinAreaPartitionConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Extraction", {
      {DETECT_MINAREA.c_str(), po::value<int>()->default_value(3), "min. # of pixels above threshold"}
  }}};
}

void MinAreaPartitionConfig::initialize(const UserValues& args) {
  if (args.count(DETECT_MINAREA) != 0) {
    auto min_pixel_count = args.at(DETECT_MINAREA).as<int>();
    getDependency<PartitionStepConfig>().addPartitionStepCreator(
            [min_pixel_count](std::shared_ptr<SourceFactory>) {
              return std::make_shared<MinAreaPartitionStep>(min_pixel_count);
            }
    );
  }
}

} // SExtractor namespace
