/*
 * AperturePhotometryTaskFactory.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"
#include "SEFramework/Aperture/CircularAperture.h"

#include "SEImplementation/Configuration/MagnitudeConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryArray.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlagTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryArrayTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryConfig.h"
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
  } else if (property_id.getTypeId() == typeid(AperturePhotometryArray)) {
    return std::make_shared<AperturePhotometryArrayTask>(
      m_apertures_per_output.at(instance),
      instance
    );
  } else if (property_id == PropertyId::create<ApertureFlag>()) {
    return std::make_shared<ApertureFlagTask>(m_all_apertures);
  }
  return nullptr;
}

void AperturePhotometryTaskFactory::registerPropertyInstances(OutputRegistry &registry) {
  std::vector<std::pair<std::string, unsigned int>> flux_instances, flux_err_instances;
  std::vector<std::pair<std::string, unsigned int>> mag_instances, mag_err_instances;
  std::vector<std::pair<std::string, unsigned int>> flags_instances;

  for (auto &aggregated_ap : m_apertures_per_output) {
    auto& array_id = aggregated_ap.first;

    std::string name = m_col_prefix.at(array_id);

    flux_instances.emplace_back(std::make_pair(name + "_flux", array_id));
    flux_err_instances.emplace_back(std::make_pair(name + "_flux_err", array_id));
    mag_instances.emplace_back(std::make_pair(name + "_mag", array_id));
    mag_err_instances.emplace_back(std::make_pair(name + "_mag_err", array_id));
    flags_instances.emplace_back(std::make_pair(name + "_flags", array_id));
  }

  registry.registerPropertyInstances<AperturePhotometryArray>("aperture_flux", flux_instances);
  registry.registerPropertyInstances<AperturePhotometryArray>("aperture_flux_err", flux_err_instances);
  registry.registerPropertyInstances<AperturePhotometryArray>("aperture_mag", mag_instances);
  registry.registerPropertyInstances<AperturePhotometryArray>("aperture_mag_err", mag_err_instances);
  registry.registerPropertyInstances<AperturePhotometryArray>("aperture_flags", flags_instances);
}

void AperturePhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
  manager.registerConfiguration<WeightImageConfig>();
  manager.registerConfiguration<AperturePhotometryConfig>();
  manager.registerConfiguration<MeasurementImageConfig>();
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto &measurement_config = manager.getConfiguration<MeasurementImageConfig>();
  auto &aperture_config = manager.getConfiguration<AperturePhotometryConfig>();
  auto measurement_images_nb = std::max<unsigned int>(1, measurement_config.getMeasurementImages().size());

  m_aperture_config = aperture_config.getApertures();
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  m_symmetry_usage = manager.getConfiguration<WeightImageConfig>().symmetryUsage();

  for (unsigned int i = 0; i < measurement_images_nb; ++i) {
    for (auto a : aperture_config.getAperturesForImage(i)) {
      if (std::find(m_all_apertures.begin(), m_all_apertures.end(), a) == m_all_apertures.end()) {
        m_all_apertures.emplace_back(a);
      }
    }
  }

  auto outputs = aperture_config.getImagesToOutput();
  unsigned i = 0;
  for (auto j = outputs.begin(); j != outputs.end(); ++i, ++j) {
    m_col_prefix[i] = j->first;
    m_apertures_per_output[i] = j->second;
  }
}

}

