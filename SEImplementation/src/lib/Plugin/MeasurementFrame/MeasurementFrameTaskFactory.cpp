/*
 * MeasurementFrameTaskFactory.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <SEImplementation/Background/BackgroundAnalyzerFactory.h>
#include <boost/filesystem.hpp>

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

    try {
      return std::make_shared<MeasurementFrameTask>(instance, m_measurement_frames.at(instance));
    } catch (const std::out_of_range&) {}
  }
  return nullptr;
}

void MeasurementFrameTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MeasurementImageConfig>();
  manager.registerConfiguration<BackgroundConfig>();
}

void MeasurementFrameTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  const auto& measurement_images = manager.getConfiguration<MeasurementImageConfig>().getMeasurementImages();
  const auto& coordinate_systems = manager.getConfiguration<MeasurementImageConfig>().getCoordinateSystems();
  const auto& weight_images = manager.getConfiguration<MeasurementImageConfig>().getWeightImages();
  const auto& is_weight_absolute = manager.getConfiguration<MeasurementImageConfig>().getAbsoluteWeights();

  const auto& gains = manager.getConfiguration<MeasurementImageConfig>().getGains();
  const auto& saturation_levels = manager.getConfiguration<MeasurementImageConfig>().getSaturationLevels();
  const auto& thresholds = manager.getConfiguration<MeasurementImageConfig>().getWeightThresholds();

  const auto& image_paths = manager.getConfiguration<MeasurementImageConfig>().getImagePaths();
  const auto& ids = manager.getConfiguration<MeasurementImageConfig>().getImageIds();

  BackgroundAnalyzerFactory background_analyzer_factory;
  background_analyzer_factory.configure(manager);
  auto background_analyzer = background_analyzer_factory.createBackgroundAnalyzer();

  for (unsigned int i=0; i<measurement_images.size(); i++) {
    auto measurement_frame = std::make_shared<MeasurementImageFrame>(
        measurement_images[i], weight_images[i], thresholds[i], coordinate_systems[i], gains[i], saturation_levels[i], false);

    auto background_model = background_analyzer->analyzeBackground(measurement_images[i], weight_images[i],
        ConstantImage<unsigned char>::create(measurement_images[i]->getWidth(),
            measurement_images[i]->getHeight(), false), measurement_frame->getVarianceThreshold());

    measurement_frame->setBackgroundLevel(background_model.getLevelMap());
    measurement_frame->setLabel(boost::filesystem::basename(image_paths[i]));

    if (weight_images[i] != nullptr) {
      if (is_weight_absolute[i]) {
        measurement_frame->setVarianceMap(weight_images[i]);
      } else {
        auto scaled_image = MultiplyImage<SeFloat>::create(weight_images[i], background_model.getScalingFactor());
        measurement_frame->setVarianceMap(scaled_image);
      }
    } else {
      measurement_frame->setVarianceMap(background_model.getVarianceMap());
    }

    m_measurement_frames[ids[i]] = measurement_frame;
  }
}

}
