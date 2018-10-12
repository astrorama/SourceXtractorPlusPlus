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
  if (property_id.getTypeId() == typeid(AperturePhotometry) && instance < m_measure_apertures.size()) {
    return std::make_shared<AperturePhotometryTask>(
      std::make_shared<CircularAperture>(m_measure_apertures[instance]),
      instance,
      m_image_instances[instance],
      m_magnitude_zero_point,
      m_symmetry_usage
    );
  } else if (property_id.getTypeId() == typeid(ApertureFlag) && instance < m_detection_apertures.size()) {
    return std::make_shared<ApertureFlagTask>(
      std::make_shared<CircularAperture>(m_detection_apertures[instance]),
      instance
    );
  } else {
    return nullptr;
  }
}

void AperturePhotometryTaskFactory::registerPropertyInstances(OutputRegistry &output_registry) {
  output_registry.registerPropertyInstances<AperturePhotometry>(m_photometry_names);
  output_registry.registerPropertyInstances<ApertureFlag>(m_flag_names);
}

void AperturePhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
  manager.registerConfiguration<AperturePhotometryConfig>();
  manager.registerConfiguration<MeasurementConfig>();
  manager.registerConfiguration<WeightImageConfig>();
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto &measurement_config = manager.getConfiguration<MeasurementConfig>();
  auto apertures = manager.getConfiguration<AperturePhotometryConfig>().getApertures();
  auto measurement_images_nb = std::max<unsigned int>(1, measurement_config.getMeasurementImages().size());

  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  m_symmetry_usage = manager.getConfiguration<WeightImageConfig>().symmetryUsage();

  // FIXME This will need to be replaced by vector output

  // We have one ApertureFlag per aperture size, since it only works on the detection image
  unsigned int flag_instance = 0;
  for (auto aperture_size : apertures) {
    m_detection_apertures.emplace_back(aperture_size);
    std::stringstream instance_name;
    instance_name << aperture_size;
    m_flag_names.emplace_back(std::make_pair(instance_name.str(), flag_instance));
  }

  // We have one AperturePhotometry per aperture per measurement image
  std::map<std::string, unsigned> pos_in_group;

  unsigned int aperture_instance_nb = 0;
  for (unsigned int image_nb = 0; image_nb < measurement_images_nb; image_nb++) {
    auto& group = measurement_config.getGroupForImage(image_nb);
    unsigned pos = pos_in_group[group->getName()]++;

    for (auto aperture_size : apertures) {
      m_aperture_instances[std::make_pair(image_nb, aperture_size)] = aperture_instance_nb;
      m_image_instances.emplace_back(image_nb);
      m_measure_apertures.emplace_back(aperture_size);


      std::stringstream instance_name;
      instance_name << group->getName() << "_" << pos << "_" << aperture_size;
      m_photometry_names.emplace_back(std::make_pair(instance_name.str(), aperture_instance_nb));

      aperture_instance_nb++;
    }
  }
}

}

