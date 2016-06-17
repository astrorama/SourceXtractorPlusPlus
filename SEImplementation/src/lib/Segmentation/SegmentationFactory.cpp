/**
 * @file src/lib/SegmentationFactory.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include "Configuration/ConfigManager.h"
#include "SEImplementation/Configuration/SegmentationConfig.h"
#include "SEImplementation/Segmentation/Lutz.h"

#include "SEImplementation/Segmentation/SegmentationFactory.h"

using namespace Euclid::Configuration;namespace SExtractor {

SegmentationFactory::SegmentationFactory(std::shared_ptr<TaskRegistry> task_registry)
: m_task_registry(task_registry) {
}

void SegmentationFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) {
  manager.registerConfiguration<SegmentationConfig>();
}

void SegmentationFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto segmentation_config = manager.getConfiguration<SegmentationConfig>();

  switch (segmentation_config.getAlgorithmOption()) {
    case SegmentationConfig::Algorithm::LUTZ:
      m_task = std::make_shared<Lutz>(m_task_registry);
      break;
    case SegmentationConfig::Algorithm::UNKNOWN:
    default:
      throw Elements::Exception("Unknown segmentation algorithm.");
  }
}

std::shared_ptr<Segmentation> SegmentationFactory::getSegmentation() const {
  return m_task;
}



} // SEImplementation namespace



