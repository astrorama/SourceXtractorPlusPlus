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

#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Configuration/MagnitudeConfig.h"
#include "SEImplementation/Configuration/MeasurementConfig.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryConfig.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> AperturePhotometryTaskFactory::createTask(const PropertyId& property_id) const {
  auto instance = property_id.getIndex();
  if (property_id.getTypeId() == typeid(AperturePhotometry) && instance < m_apertures.size()) {
    return std::make_shared<AperturePhotometryTask>(
        std::make_shared<CircularAperture>(m_apertures[instance]),
        instance,
        m_image_instances[instance],
        m_magnitude_zero_point,
        m_symmetry_usage
        );
  } else {
    return nullptr;
  }
}

void AperturePhotometryTaskFactory::registerPropertyInstances(OutputRegistry& output_registry) {
  output_registry.registerPropertyInstances<AperturePhotometry>(m_instance_names);
}

void AperturePhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
  manager.registerConfiguration<AperturePhotometryConfig>();
  manager.registerConfiguration<MeasurementConfig>();
  manager.registerConfiguration<WeightImageConfig>();
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& measurement_config = manager.getConfiguration<MeasurementConfig>();
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  m_symmetry_usage = manager.getConfiguration<WeightImageConfig>().symmetryUsage();
  auto apertures = manager.getConfiguration<AperturePhotometryConfig>().getApertures();

  auto measurement_images_nb = std::max<unsigned int>(1, measurement_config.getMeasurementImages().size());
  std::cout << "measurement_images_nb: " << measurement_images_nb << std::endl;

  // determine which apertures are needed for each image
  std::vector<std::set<SeFloat>> aperture_sizes(measurement_images_nb);

  for (auto& group : measurement_config.getImageGroups()) {
    auto apertures = group->getAperturePhotometryOptions().getApertureSizes();

    for (auto image_nb : group->getMeasurementImageIndices()) {
      std::copy(apertures.begin(), apertures.end(), std::inserter(aperture_sizes[image_nb], aperture_sizes[image_nb].end()));
    }
  }

  unsigned int aperture_instance_nb = 0;
  for (unsigned int image_nb = 0; image_nb < measurement_images_nb; image_nb++) {
    for (auto aperture_size : aperture_sizes[image_nb]) {
      m_aperture_instances[std::make_pair(image_nb, aperture_size)] = aperture_instance_nb;
      m_image_instances.emplace_back(image_nb);
      m_apertures.emplace_back(aperture_size);

      // FIXME This will need to be replaced by vector output
      std::stringstream instance_name;
      instance_name << image_nb << "_" << aperture_size;
      m_instance_names.emplace_back(std::make_pair(instance_name.str(), aperture_instance_nb));

      aperture_instance_nb++;
    }
  }
}

}

