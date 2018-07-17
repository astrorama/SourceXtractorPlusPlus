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

#include "SEImplementation/Configuration/MagnitudeConfig.h"

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
        true
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
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  auto apertures = manager.getConfiguration<AperturePhotometryConfig>().getApertures();

  // this code is for the detection image only,
  // hence images_nb = 1
  int detection_image_nb = 1;

  // determine which apertures are needed for each image
  std::vector<std::set<SeFloat>> aperture_sizes(detection_image_nb);

  // copy that stuff over
  std::copy(apertures.begin(), apertures.end(), std::inserter(aperture_sizes[0], aperture_sizes[0].end()));

  unsigned int aperture_instance_nb = 0;
  for (unsigned int image_nb = 0; image_nb < detection_image_nb; image_nb++) {
    for (auto aperture_size : aperture_sizes[image_nb]) {
      m_aperture_instances[std::make_pair(image_nb, aperture_size)] = aperture_instance_nb;
      m_image_instances.emplace_back(image_nb);
      m_apertures.emplace_back(aperture_size);

      // This will need to be replaced by vector output
      std::stringstream instance_name;
      // naming for the multi-frame mode:
      // instance_name << image_nb << "_" << aperture_size;
      // naming for single image mode:
      instance_name << aperture_size;
      m_instance_names.emplace_back(std::make_pair(instance_name.str(), aperture_instance_nb));

      aperture_instance_nb++;
    }
  }

}

}

