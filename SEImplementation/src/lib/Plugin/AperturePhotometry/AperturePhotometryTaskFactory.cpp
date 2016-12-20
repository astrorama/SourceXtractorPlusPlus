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
        m_magnitude_zero_point);
  } else if(m_aggregate_tasks_map.find(instance) != m_aggregate_tasks_map.end()) {
    return m_aggregate_tasks_map.at(instance);
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
}

void AperturePhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& measurement_config = manager.getConfiguration<MeasurementConfig>();
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  auto apertures = manager.getConfiguration<AperturePhotometryConfig>().getApertures();

  auto measurement_images_nb = std::max<unsigned int>(1, measurement_config.getMeasurementImages().size());
  std::cout << "measurement_images_nb: " << measurement_images_nb << std::endl;

  for (unsigned int image_nb = 0; image_nb < measurement_images_nb; image_nb++) {
    for (unsigned int aperture_size_index = 0; aperture_size_index < apertures.size(); aperture_size_index++) {
      m_apertures.emplace_back(apertures[aperture_size_index]);
      //m_instance_names.emplace_back(std::make_pair(instance_name.str(), image_nb));
      m_image_instances.emplace_back(image_nb);

//      std::cout << ": " << instance_name.str() << std::endl;
    }
  }

  unsigned int instance_nb = m_instance_names.size();

  for (auto& group : measurement_config.getImageGroups()) {
    std::cout << "AperturePhotometryTaskFactory: image group" << std::endl;

    auto aperture_photometry_options = group->getAperturePhotometryOptions();

    if (aperture_photometry_options.getAggregateType() == MeasurementConfig::AperturePhotometryOptions::AggregateType::None) {
      for (auto image_nb : group->getMeasurementImageIndices()) {
        for (unsigned int aperture_size_index = 0; aperture_size_index < apertures.size(); aperture_size_index++) {
          std::stringstream instance_name;
          instance_name << group->getName() << "_" << image_nb << "_" << aperture_size_index;
          m_instance_names.emplace_back(std::make_pair(instance_name.str(), image_nb * apertures.size() + aperture_size_index));
        }
      }
    } else {
      for (unsigned int aperture_size_index = 0; aperture_size_index < apertures.size(); aperture_size_index++) {
        auto indices = group->getMeasurementImageIndices();
        std::vector<unsigned int> instances(indices.begin(), indices.end());
        std::for_each(instances.begin(), instances.end(),
            [&](unsigned int& instance) { instance = instance * apertures.size() + aperture_size_index; }
        );

        m_aggregate_tasks_map[instance_nb] = std::make_shared<AperturePhotometryAggregateTask>(
            instance_nb,
            instances,
            0
        );

        std::stringstream instance_name;
        std::string group_name = group->getName();
        instance_name << group_name << "_mean";
        if (apertures.size() > 1) {
          instance_name << "_" << aperture_size_index;
        }

        m_instance_names.emplace_back(std::make_pair(instance_name.str(), instance_nb));

  //      std::cout << ": " << instance_name.str() << std::endl;

        instance_nb++;
      }
    }
  }
}

}

