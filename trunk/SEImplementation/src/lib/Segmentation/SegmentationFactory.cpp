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
    : m_filtering_enabled(true),
      m_algorithm(SegmentationConfig::Algorithm::UNKNOWN),
      m_task_provider(task_provider) {
}

void SegmentationFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<SegmentationConfig>();
}

void SegmentationFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto segmentation_config = manager.getConfiguration<SegmentationConfig>();
  m_algorithm = segmentation_config.getAlgorithmOption();
  m_filtering_enabled = segmentation_config.isFilteringEnabled();
}

std::shared_ptr<Segmentation> SegmentationFactory::createSegmentation() const {
  // TEST: simple hard-coded 3x3 pyramid filter
  auto convolution_filter = std::make_shared<VectorImage<SeFloat>>(3, 3);
  convolution_filter->setValue(0,0, 1);
  convolution_filter->setValue(0,1, 2);
  convolution_filter->setValue(0,2, 1);

  convolution_filter->setValue(1,0, 2);
  convolution_filter->setValue(1,1, 4);
  convolution_filter->setValue(1,2, 2);

  convolution_filter->setValue(2,0, 1);
  convolution_filter->setValue(2,1, 2);
  convolution_filter->setValue(2,2, 1);

  auto image_processing_list = std::make_shared<DetectionImageProcessingList>(
      m_filtering_enabled ?
        std::vector<std::shared_ptr<DetectionImageProcessing>>  {
          std::make_shared<BackgroundConvolution>(convolution_filter),
        }
      :
        std::vector<std::shared_ptr<DetectionImageProcessing>>  {}
  );

  auto segmentation = std::make_shared<Segmentation>(image_processing_list);
  switch (m_algorithm) {
    case SegmentationConfig::Algorithm::LUTZ:
      segmentation->setLabelling<LutzSegmentation>(std::make_shared<SourceWithOnDemandPropertiesFactory>(m_task_provider));
      break;
    case SegmentationConfig::Algorithm::UNKNOWN:
    default:
      throw Elements::Exception("Unknown segmentation algorithm.");
  }

  return segmentation;
}

} // SEImplementation namespace
