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
 * CoreThresholdPartitionConfig.cpp
 *
 *  Created on: May 28, 2019
 *      Author: mkuemmel@usm.lmu.de
 */

#include <Configuration/ProgramOptionsHelper.h>

#include "SEImplementation/Plugin/CoreThresholdPartition/CoreThresholdPartitionConfig.h"

#include "SEImplementation/Plugin/CoreThresholdPartition/CoreThresholdPartitionStep.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"
#include "SEImplementation/Configuration/MultiThresholdPartitionConfig.h"
#include "SEImplementation/Configuration/DetectionFrameConfig.h"


namespace po = boost::program_options;
using namespace Euclid::Configuration;

namespace SourceXtractor {

static const std::string CORE_THRESHOLD   {"core-threshold-value" };
static const std::string CORE_MINAREA     {"core-minimum-area" };
static const std::string CORE_THRESH_USE {"partition-corethreshold" };

CoreThresholdPartitionConfig::CoreThresholdPartitionConfig(long manager_id)
  : Configuration(manager_id), m_core_threshold(0.), m_core_minarea(0) {
  declareDependency<PartitionStepConfig>();
  declareDependency<DetectionFrameConfig>();

  // this is used to enforce the order the PartitionSteps are added and performed
  ConfigManager::getInstance(manager_id).registerDependency<CoreThresholdPartitionConfig, MultiThresholdPartitionConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> CoreThresholdPartitionConfig::getProgramOptions() {
  return {{"Core threshold partitioning", {
      {CORE_THRESHOLD.c_str(), po::value<double>()->default_value(0.0), "The core threshold level"},
      {CORE_MINAREA.c_str(), po::value<int>()->default_value(0), "The minimum pixel area for partitioning"},
      {CORE_THRESH_USE.c_str(), po::value<bool>()->default_value(false), "Activate core threshold partitioning"}
  }}};
}

void CoreThresholdPartitionConfig::initialize(const UserValues &args) {
  m_core_threshold = args.find(CORE_THRESHOLD)->second.as<double>();
  m_core_minarea   = args.find(CORE_MINAREA)->second.as<int>();

  if (m_core_threshold < 0.) {
    throw Elements::Exception() << "Invalid " << CORE_THRESHOLD << " value: " << m_core_threshold;
  }
  if (m_core_minarea < 0) {
    throw Elements::Exception() << "Invalid " << CORE_MINAREA << " value: " << m_core_minarea;
  }

  if (getDependency<DetectionFrameConfig>().getDetectionFrames().size() > 0) {
    if (m_core_threshold > 0.0 && m_core_minarea > 0 && args.at(CORE_THRESH_USE).as<bool>()) {
      double core_threshold = m_core_threshold;
      int core_minarea      = m_core_minarea;
      getDependency<PartitionStepConfig>().addPartitionStepCreator([core_threshold, core_minarea](std::shared_ptr<SourceFactory>)
          { return std::make_shared<CoreThresholdPartitionStep>(core_threshold, core_minarea); } );
    }
  }
}

const double& CoreThresholdPartitionConfig::getCoreThreshold() const {
  return m_core_threshold;
}

const int& CoreThresholdPartitionConfig::getCoreMinArea() const {
  return m_core_minarea;
}

} // end SExtractor
