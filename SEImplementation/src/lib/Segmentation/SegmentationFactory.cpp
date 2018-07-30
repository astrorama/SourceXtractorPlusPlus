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

#include "SEImplementation/Segmentation/SegmentationFactory.h"

using namespace Euclid::Configuration;

namespace SExtractor {

SegmentationFactory::SegmentationFactory(std::shared_ptr<TaskProvider> task_provider)
    : m_algorithm(SegmentationConfig::Algorithm::UNKNOWN),
      m_task_provider(task_provider) {
}

void SegmentationFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<SegmentationConfig>();
}

void SegmentationFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto segmentation_config = manager.getConfiguration<SegmentationConfig>();
  m_algorithm = segmentation_config.getAlgorithmOption();
  m_filter = segmentation_config.getFilter();
}

std::shared_ptr<Segmentation> SegmentationFactory::createSegmentation() const {

  auto segmentation = std::make_shared<Segmentation>(m_filter);
  switch (m_algorithm) {
    case SegmentationConfig::Algorithm::LUTZ:
      //FIXME Use a factory from parameter
      segmentation->setLabelling<LutzSegmentation>(std::make_shared<SourceWithOnDemandPropertiesFactory>(m_task_provider));
      break;
    case SegmentationConfig::Algorithm::UNKNOWN:
    default:
      throw Elements::Exception("Unknown segmentation algorithm.");
  }

  return segmentation;
}

} // SEImplementation namespace
