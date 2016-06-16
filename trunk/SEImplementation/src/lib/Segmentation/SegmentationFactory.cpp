/**
 * @file src/lib/SegmentationFactory.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"
#include "SEConfig/SegmentationConfig.h"
#include "SEImplementation/Segmentation/Lutz.h"

#include "SEImplementation/Segmentation/SegmentationFactory.h"

using namespace Euclid::Configuration;namespace SExtractor {

SegmentationFactory::SegmentationFactory(std::shared_ptr<TaskRegistry> task_registry)
: m_task_registry(task_registry) {}

std::shared_ptr<Segmentation> SegmentationFactory::getSegmentation() const {
  auto segmentation_config = ConfigManager::getInstance(0).getConfiguration<SegmentationConfig>();

  switch (segmentation_config.getAlgorithmOption()) {
    default:
    case SegmentationConfig::Algorithm::LUTZ:
      return std::make_shared<Lutz>(m_task_registry);
  }
}



} // SEImplementation namespace



