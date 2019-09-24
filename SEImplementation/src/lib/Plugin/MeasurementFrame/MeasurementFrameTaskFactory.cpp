/*
 * MeasurementFrameTaskFactory.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <sstream>

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
  const auto& image_infos = manager.getConfiguration<MeasurementImageConfig>().getImageInfos();

  BackgroundAnalyzerFactory background_analyzer_factory;
  background_analyzer_factory.configure(manager);
  for (auto& image_info : image_infos) {
    auto measurement_frame = std::make_shared<MeasurementImageFrame>(
        image_info.m_measurement_image,
        image_info.m_weight_image,
        image_info.m_weight_threshold,
        image_info.m_coordinate_system,
        image_info.m_gain,
        image_info.m_saturation_level,
        false);

    auto background_analyzer = background_analyzer_factory.createBackgroundAnalyzer(image_info.m_weight_type);
    auto background_model = background_analyzer->analyzeBackground(
        image_info.m_measurement_image,
        image_info.m_weight_image,
        ConstantImage<unsigned char>::create(image_info.m_measurement_image->getWidth(),
            image_info.m_measurement_image->getHeight(), false),
        measurement_frame->getVarianceThreshold());

    if (image_info.m_is_background_constant) {
      measurement_frame->setBackgroundLevel(image_info.m_constant_background_value);
    } else {
      measurement_frame->setBackgroundLevel(background_model.getLevelMap());
    }

    std::stringstream label;
    label << boost::filesystem::basename(image_info.m_path) << "_" << image_info.m_image_hdu;
    measurement_frame->setLabel(label.str());

    if (image_info.m_weight_image != nullptr) {
      if (image_info.m_absolute_weight) {
        measurement_frame->setVarianceMap(image_info.m_weight_image);
      } else {
        auto scaled_image = MultiplyImage<SeFloat>::create(
            image_info.m_weight_image,
            background_model.getScalingFactor());
        measurement_frame->setVarianceMap(scaled_image);
      }
    } else {
      measurement_frame->setVarianceMap(background_model.getVarianceMap());
    }

    m_measurement_frames[image_info.m_id] = measurement_frame;
  }
}

}
