/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file SourceFlagsTaskFactory.cpp
 *
 * @date Jul 12, 2018
 * @author mkuemmel@usm.lmu.de
 */

#include "SEImplementation/Configuration/MeasurementConfig.h"
#include "SEImplementation/Plugin/SourceFlags/SourceFlagsTaskFactory.h"
#include "SEImplementation/Plugin/SourceFlags/SourceFlagsSourceTask.h"

namespace SExtractor {

void SourceFlagsTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MeasurementConfig>();
}

void SourceFlagsTaskFactory::registerPropertyInstances(SExtractor::OutputRegistry &output_registry) {
  output_registry.registerPropertyInstances<SourceFlags>(m_instance_names);
}

void SourceFlagsTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  auto& measurement_config = manager.getConfiguration<MeasurementConfig>();
  auto measurement_images_nb = std::max<unsigned int>(1, measurement_config.getMeasurementImages().size());

  for (unsigned i = 0; i < measurement_images_nb; ++i) {
    m_instance_names.emplace_back(std::make_pair(std::to_string(i), i));
  }
}

std::shared_ptr<Task> SourceFlagsTaskFactory::createTask(const PropertyId &property_id) const {
  if (property_id.getTypeId() == typeid(SourceFlags)) {
    return std::make_shared<SourceFlagsSourceTask>(property_id.getIndex());
  } else {
    return nullptr;
  }
}

} // end SExtractor
