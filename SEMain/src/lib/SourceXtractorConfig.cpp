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
/**
 * @file src/lib/SourceXtractorConfig.cpp
 * @date 07/20/16
 * @author nikoapos
 */

#include <SEMain/SourceXtractorConfig.h>
#include "SEMain/PluginConfig.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"

#include "SEImplementation/Configuration/BackgroundConfig.h"
#include "SEImplementation/Configuration/MinAreaPartitionConfig.h"
#include "SEImplementation/Configuration/AttractorsPartitionConfig.h"
#include "SEImplementation/Configuration/MultiThresholdPartitionConfig.h"
#include "SEImplementation/Configuration/CleaningConfig.h"

namespace po = boost::program_options;

namespace SourceXtractor {

SourceXtractorConfig::SourceXtractorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<DetectionImageConfig>();
  declareDependency<WeightImageConfig>();

  declareDependency<BackgroundConfig>();
  declareDependency<MinAreaPartitionConfig>();
  declareDependency<AttractorsPartitionConfig>();
  declareDependency<MultiThresholdPartitionConfig>();
  declareDependency<CleaningConfig>();
  // The following dependency is not used but it is included so its options
  // are included in the help message
  declareDependency<PluginConfig>();
}

} // SourceXtractor namespace



