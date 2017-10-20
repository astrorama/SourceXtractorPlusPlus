/*
 * MeasurementFrameTaskFactory.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Configuration/MeasurementConfig.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTask.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTaskFactory.h"

#include "SEFramework/Image/ConstantImage.h"
#include "SEImplementation/Background/SimpleBackgroundRMSAnalyzer.h"

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
  const auto& absolute_weights = manager.getConfiguration<MeasurementConfig>().getAbsoluteWeights();

  SimpleBackgroundRMSAnalyzer analyzer;

  for (unsigned int i=0; i<measurement_images.size(); i++) {
    auto measurement_frame = std::make_shared<MeasurementImageFrame>(
        measurement_images[i], weight_images[i], absolute_weights[i], 9999999, coordinate_systems[i], 1, 65000); // FIXME !!! we need weight threshold

    auto background_rms_map = analyzer.analyzeBackground(measurement_images[i], weight_images[i],
        ConstantImage<unsigned char>::create(measurement_images[i]->getWidth(),
            measurement_images[i]->getHeight(), true));
    measurement_frame->setBackgroundRMS(background_rms_map);

    // FIXME gain/saturation for measurement images

    m_measurement_frames.emplace_back(measurement_frame);
  }
}

}
