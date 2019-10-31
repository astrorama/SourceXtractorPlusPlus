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
 * @file AttractorsPartitionConfig.cpp
 * @author nikoapos
 */

#include "SEImplementation/Configuration/AttractorsPartitionConfig.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"
#include "SEImplementation/Configuration/MinAreaPartitionConfig.h"

#include "SEImplementation/Partition/AttractorsPartitionStep.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string USE_ATTRACTORS_PARTITION {"use-attractors-partition"};

AttractorsPartitionConfig::AttractorsPartitionConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PartitionStepConfig>();
  ConfigManager::getInstance(manager_id).registerDependency<AttractorsPartitionConfig, MinAreaPartitionConfig>();

}

auto AttractorsPartitionConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Extraction", {
      {USE_ATTRACTORS_PARTITION.c_str(), po::bool_switch(),
              "Enables the use of attractors for partitioning"}
  }}};
}

void AttractorsPartitionConfig::initialize(const UserValues& args) {
  if (args.at(USE_ATTRACTORS_PARTITION).as<bool>()) {
    getDependency<PartitionStepConfig>().addPartitionStepCreator(
            [](std::shared_ptr<SourceFactory> source_factory) {
              return std::make_shared<AttractorsPartitionStep>(source_factory);
            }
    );
  }
}

} // SourceXtractor namespace
