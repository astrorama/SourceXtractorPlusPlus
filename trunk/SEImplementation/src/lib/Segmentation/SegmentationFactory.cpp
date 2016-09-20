/**
 * @file src/lib/SegmentationFactory.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"

#include "SEFramework/Source/SourceWithOnDemandPropertiesFactory.h"

#include "SEFramework/Image/ImageProcessingList.h"

#include "SEImplementation/Configuration/SegmentationConfig.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"

#include "SEImplementation/Segmentation/Lutz.h"
#include "SEImplementation/Segmentation/BackgroundSubtract.h"

#include "SEImplementation/Segmentation/SegmentationFactory.h"

using namespace Euclid::Configuration;

namespace SExtractor {

SegmentationFactory::SegmentationFactory(std::shared_ptr<TaskProvider> task_provider)
    : m_task_provider(task_provider) {
}

void SegmentationFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<SegmentationConfig>();
  manager.registerConfiguration<BackgroundConfig>();
}

void SegmentationFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto segmentation_config = manager.getConfiguration<SegmentationConfig>();
  auto background_config = manager.getConfiguration<BackgroundConfig>();

  auto image_processing_list = std::make_shared<DetectionImageProcessingList>(
      std::vector<std::shared_ptr<DetectionImageProcessing>>  {
        std::make_shared<BackgroundSubtract>(background_config.getBackgroundValue())
      }
  );

  m_segmentation = std::make_shared<Segmentation>(image_processing_list, nullptr);
  switch (segmentation_config.getAlgorithmOption()) {
    case SegmentationConfig::Algorithm::LUTZ:
      m_segmentation->setLabelling<Lutz>(std::make_shared<SourceWithOnDemandPropertiesFactory>(m_task_provider));
      break;
    case SegmentationConfig::Algorithm::UNKNOWN:
    default:
      throw Elements::Exception("Unknown segmentation algorithm.");
  }
}

std::shared_ptr<Segmentation> SegmentationFactory::getSegmentation() const {
  return m_segmentation;
}

} // SEImplementation namespace
