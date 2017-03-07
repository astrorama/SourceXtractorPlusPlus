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

#include "SEImplementation/Configuration/BackgroundConfig.h"

#include "SEImplementation/Segmentation/BackgroundSubtract.h"
#include "SEImplementation/Segmentation/BackgroundConvolution.h"

#include "SEImplementation/Segmentation/SegmentationFactory.h"

#include "SEImplementation/Segmentation/LutzSegmentation.h"

using namespace Euclid::Configuration;

namespace SExtractor {

SegmentationFactory::SegmentationFactory(std::shared_ptr<TaskProvider> task_provider)
    : m_background_absolute(false),
      m_background_value(0),
      m_threshold_absolute(false),
      m_threshold_value(0),
      m_filtering_enabled(true),
      m_algorithm(SegmentationConfig::Algorithm::UNKNOWN),
      m_task_provider(task_provider) {
}

void SegmentationFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<SegmentationConfig>();
  manager.registerConfiguration<BackgroundConfig>();
}

void SegmentationFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto segmentation_config = manager.getConfiguration<SegmentationConfig>();
  auto background_config = manager.getConfiguration<BackgroundConfig>();

  m_background_absolute = background_config.isBackgroundAbsolute();
  m_background_value = background_config.getBackgroundValue();
  m_threshold_absolute = background_config.isThresholdAbsolute();
  m_threshold_value = background_config.getThresholdValue();
  m_algorithm = segmentation_config.getAlgorithmOption();
  m_filtering_enabled = segmentation_config.isFilteringEnabled();
}

std::shared_ptr<Segmentation> SegmentationFactory::createSegmentation(SeFloat background_value, SeFloat threshold) const {

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

  auto actual_background_value = m_background_absolute ? m_background_value : background_value;
  auto actual_threshold_value = m_threshold_absolute ? m_threshold_value : threshold;

  std::cout << "background: " <<  actual_background_value << " threshold: "  << actual_threshold_value << '\n';


  auto image_processing_list = std::make_shared<DetectionImageProcessingList>(
      std::vector<std::shared_ptr<DetectionImageProcessing>>  {
        std::make_shared<BackgroundSubtract>(actual_background_value),
      }
  );

  auto labelling_processing_list = std::make_shared<DetectionImageProcessingList>(
      m_filtering_enabled ?
        std::vector<std::shared_ptr<DetectionImageProcessing>>  {
          std::make_shared<BackgroundConvolution>(convolution_filter),
        }
      :
        std::vector<std::shared_ptr<DetectionImageProcessing>>  {}
  );

  auto thresholding_processing_list = std::make_shared<DetectionImageProcessingList>(
      std::vector<std::shared_ptr<DetectionImageProcessing>>  {
        std::make_shared<BackgroundSubtract>(actual_threshold_value)
      }
  );

  auto segmentation = std::make_shared<Segmentation>(image_processing_list, labelling_processing_list, thresholding_processing_list);
  switch (m_algorithm) {
    case SegmentationConfig::Algorithm::LUTZ:
      segmentation->setLabelling<LutzSegmentation>(std::make_shared<SourceWithOnDemandPropertiesFactory>(m_task_provider), actual_threshold_value);
      break;
    case SegmentationConfig::Algorithm::UNKNOWN:
    default:
      throw Elements::Exception("Unknown segmentation algorithm.");
  }

  return segmentation;
}

} // SEImplementation namespace
