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
 * @file src/lib/SegmentationFactory.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include <iostream>

#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Source/SourceWithOnDemandPropertiesFactory.h"
#include "SEFramework/Image/ImageProcessingList.h"

#include "SEImplementation/Segmentation/BackgroundConvolution.h"
#include "SEImplementation/Segmentation/LutzSegmentation.h"
#include "SEImplementation/Segmentation/BFSSegmentation.h"
#include "SEImplementation/Segmentation/AssocSegmentation.h"

#ifdef WITH_ML_SEGMENTATION
#include "SEImplementation/Segmentation/MLSegmentation.h"
#endif

#include "SEImplementation/Segmentation/SegmentationFactory.h"

using namespace Euclid::Configuration;

namespace SourceXtractor {

SegmentationFactory::SegmentationFactory(std::shared_ptr<TaskProvider> task_provider)
    : m_algorithm(SegmentationConfig::Algorithm::UNKNOWN),
      m_task_provider(task_provider), m_lutz_window_size(0), m_bfs_max_delta(0), m_ml_threshold(0.) {
}

void SegmentationFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<AssocModeConfig>();
  manager.registerConfiguration<SegmentationConfig>();
//
//  manager.registerDependency<AttractorsPartitionConfig, MinAreaPartitionConfig>();
//  declareDependency<PartitionStepConfig>();
}

void SegmentationFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto segmentation_config = manager.getConfiguration<SegmentationConfig>();
  m_algorithm = segmentation_config.getAlgorithmOption();
  m_filter = segmentation_config.getFilter();
  m_lutz_window_size = segmentation_config.getLutzWindowSize();
  m_bfs_max_delta = segmentation_config.getBfsMaxDelta();
  m_model_path = segmentation_config.getOnnxModelPath();
  m_ml_threshold = segmentation_config.getMLThreashold();

  auto assoc_config = manager.getConfiguration<AssocModeConfig>();
  m_catalogs = assoc_config.getCatalogs();
}

std::shared_ptr<Segmentation> SegmentationFactory::createSegmentation() const {

  auto segmentation = std::make_shared<Segmentation>(m_filter);
  switch (m_algorithm) {
    case SegmentationConfig::Algorithm::LUTZ:
      //FIXME Use a factory from parameter
      segmentation->setLabelling<LutzSegmentation>(
          std::make_shared<SourceWithOnDemandPropertiesFactory>(m_task_provider), m_lutz_window_size);
      break;
    case SegmentationConfig::Algorithm::BFS:
      segmentation->setLabelling<BFSSegmentation>(
          std::make_shared<SourceWithOnDemandPropertiesFactory>(m_task_provider), m_bfs_max_delta);
      break;
#ifdef WITH_ML_SEGMENTATION
    case SegmentationConfig::Algorithm::ML:
      segmentation->setLabelling<MLSegmentation>(
          std::make_shared<SourceWithOnDemandPropertiesFactory>(m_task_provider), m_model_path, m_ml_threshold);
      break;
#endif
    case SegmentationConfig::Algorithm::ASSOC:
      segmentation->setLabelling<AssocSegmentation>(
          std::make_shared<SourceWithOnDemandPropertiesFactory>(m_task_provider));
      break;
    case SegmentationConfig::Algorithm::UNKNOWN:
    default:
      throw Elements::Exception("Unknown segmentation algorithm.");
  }

  return segmentation;
}

} // SEImplementation namespace
