/*
 * AperturePhotometryTaskFactory.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"
#include "SEFramework/Aperture/CircularAperture.h"

#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Configuration/MagnitudeConfig.h"
#include "SEImplementation/Configuration/MeasurementConfig.h"

#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlagTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryConfig.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> AperturePhotometryTaskFactory::createTask(const PropertyId &property_id) const {
  auto instance = property_id.getIndex();

  if (property_id.getTypeId() == typeid(AperturePhotometry)) {
    return std::make_shared<AperturePhotometryTask>(
      m_apertures,
      instance,
      m_magnitude_zero_point,
      m_symmetry_usage
    );
  }
  else if (property_id == PropertyId::create<ApertureFlag>()) {
    return std::make_shared<ApertureFlagTask>(m_apertures);
  }
  return nullptr;
}

void AperturePhotometryTaskFactory::registerPropertyInstances(OutputRegistry &output_registry) {
  output_registry.registerPropertyInstances<AperturePhotometry>(m_instances);
}

void AperturePhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
  manager.registerConfiguration<AperturePhotometryConfig>();
  manager.registerConfiguration<MeasurementConfig>();
  manager.registerConfiguration<WeightImageConfig>();
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto &measurement_config = manager.getConfiguration<MeasurementConfig>();
  auto measurement_images_nb = std::max<unsigned int>(1, measurement_config.getMeasurementImages().size());

  m_apertures = manager.getConfiguration<AperturePhotometryConfig>().getApertures();
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  m_symmetry_usage = manager.getConfiguration<WeightImageConfig>().symmetryUsage();

  for (int i = 0; i < measurement_images_nb; ++i) {
    m_instances.emplace_back(std::make_pair(std::to_string(i), i));
  }
}

}

