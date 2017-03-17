/*
 * MeasurementFrameTaskFactory.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/MeasurementConfig.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTask.h"
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
  manager.registerConfiguration<MeasurementConfig>();
}

void MeasurementFrameTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  const auto& measurement_images = manager.getConfiguration<MeasurementConfig>().getMeasurementImages();
  const auto& coordinate_systems = manager.getConfiguration<MeasurementConfig>().getCoordinateSystems();
  const auto& weight_images = manager.getConfiguration<MeasurementConfig>().getWeightImages();

  for (unsigned int i=0; i<measurement_images.size(); i++) {
    m_measurement_frames.emplace_back(std::make_shared<MeasurementImageFrame>(
        measurement_images[i], weight_images[i], coordinate_systems[i]));
  }
}

}
