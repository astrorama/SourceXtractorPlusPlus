/*
 * MeasurementFrameTaskFactory.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <SEImplementation/Background/BackgroundAnalyzerFactory.h>

#include "SEImplementation/Configuration/MeasurementImageConfig.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTask.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTaskFactory.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> MeasurementFrameTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == PropertyId::create<MeasurementFrame>().getTypeId()) {
    auto instance = property_id.getIndex();

    if (instance < m_measurement_frames.size()) {
      return std::make_shared<MeasurementFrameTask>(instance, m_measurement_frames[instance]);
    } else if (instance == 0) {
      // By default if no measurement image is provided we use the detection image as the first measurement image
      return std::make_shared<DefaultMeasurementFrameTask>(instance);
    } else {
      return nullptr;
    }
  } else {
    return nullptr;
  }
}

void MeasurementFrameTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MeasurementImageConfig>();
  manager.registerConfiguration<BackgroundConfig>();
}

void MeasurementFrameTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  const auto& measurement_images = manager.getConfiguration<MeasurementImageConfig>().getMeasurementImages();
  const auto& coordinate_systems = manager.getConfiguration<MeasurementImageConfig>().getCoordinateSystems();
  const auto& weight_images = manager.getConfiguration<MeasurementImageConfig>().getWeightImages();

  const auto& gains = manager.getConfiguration<MeasurementImageConfig>().getGains();
  const auto& saturation_levels = manager.getConfiguration<MeasurementImageConfig>().getSaturationLevels();
  const auto& thresholds = manager.getConfiguration<MeasurementImageConfig>().getWeightThresholds();

  BackgroundAnalyzerFactory background_analyzer_factory;
  background_analyzer_factory.configure(manager);
  auto background_analyzer = background_analyzer_factory.createBackgroundAnalyzer();

  for (unsigned int i=0; i<measurement_images.size(); i++) {
    auto measurement_frame = std::make_shared<MeasurementImageFrame>(
        measurement_images[i], weight_images[i], thresholds[i], coordinate_systems[i], gains[i], saturation_levels[i], false);

    auto background_model = background_analyzer->analyzeBackground(measurement_images[i], weight_images[i],
        ConstantImage<unsigned char>::create(measurement_images[i]->getWidth(),
            measurement_images[i]->getHeight(), false), measurement_frame->getVarianceThreshold());

    measurement_frame->setVarianceMap(background_model.getVarianceMap());
    measurement_frame->setBackgroundLevel(background_model.getLevelMap());

    m_measurement_frames.emplace_back(measurement_frame);
  }
}

}
