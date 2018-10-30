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

#include "SEImplementation/Configuration/MagnitudeConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlagTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"
#include "SEImplementation/Configuration/ApertureConfig.h"
#include "SEImplementation/Configuration/MeasurementImageConfig.h"

namespace SExtractor {

std::shared_ptr<Task> AperturePhotometryTaskFactory::createTask(const PropertyId &property_id) const {
  auto instance = property_id.getIndex();

  if (property_id.getTypeId() == typeid(AperturePhotometry)) {
    return std::make_shared<AperturePhotometryTask>(
      m_aperture_config.at(instance),
      instance,
      m_magnitude_zero_point,
      m_symmetry_usage
    );
  }
  else if (property_id == PropertyId::create<ApertureFlag>()) {
    return std::make_shared<ApertureFlagTask>(m_all_apertures);
  }
  return nullptr;
}

void AperturePhotometryTaskFactory::registerPropertyInstances(OutputRegistry &output_registry) {
  output_registry.registerPropertyInstances<AperturePhotometry>(m_instances);
}

void AperturePhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
  manager.registerConfiguration<WeightImageConfig>();
  manager.registerConfiguration<ApertureConfig>();
  manager.registerConfiguration<MeasurementImageConfig>();
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto &measurement_config = manager.getConfiguration<MeasurementImageConfig>();
  auto &aperture_config = manager.getConfiguration<ApertureConfig>();
  auto measurement_images_nb = std::max<unsigned int>(1, measurement_config.getMeasurementImages().size());

  m_aperture_config = aperture_config.getApertures();
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  m_symmetry_usage =  manager.getConfiguration<WeightImageConfig>().symmetryUsage();

  for (unsigned int i = 0; i < measurement_images_nb; ++i) {
    for (auto a : aperture_config.getAperturesForImage(i)) {
      if (std::find(m_all_apertures.begin(), m_all_apertures.end(), a) == m_all_apertures.end()) {
        m_all_apertures.emplace_back(a);
      }
    }
  }

  for (auto i : aperture_config.getOutputForImages()) {
    m_instances.emplace_back(std::make_pair(std::to_string(i), i));
  }
}

}

