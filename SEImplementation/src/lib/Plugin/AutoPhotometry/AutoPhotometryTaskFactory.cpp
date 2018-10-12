/*
 * AutoPhotometryTaskFactory.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include <iostream>
#include <sstream>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Task/Task.h"


#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Configuration/MagnitudeConfig.h"

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlag.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryConfig.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTask.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlagTask.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTaskFactory.h"
#include "SEImplementation/Configuration/CheckImagesConfig.h"
#include "SEImplementation/CheckImages/SourceIdCheckImage.h"
#include "SEImplementation/Configuration/MeasurementConfig.h"


namespace SExtractor {

void AutoPhotometryTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const {
  manager.registerConfiguration<MagnitudeConfig>();
  manager.registerConfiguration<AutoPhotometryConfig>();
  manager.registerConfiguration<WeightImageConfig>();
  manager.registerConfiguration<CheckImagesConfig>();
  manager.registerConfiguration<MeasurementConfig>();
}

void AutoPhotometryTaskFactory::configure(Euclid::Configuration::ConfigManager &manager) {
  m_magnitude_zero_point = manager.getConfiguration<MagnitudeConfig>().getMagnitudeZeroPoint();
  m_kron_factor = manager.getConfiguration<AutoPhotometryConfig>().getAutoKronFactor();
  m_kron_minrad = manager.getConfiguration<AutoPhotometryConfig>().getAutoKronMinrad();
  m_symmetry_usage = manager.getConfiguration<WeightImageConfig>().symmetryUsage();

  auto &measurement_config = manager.getConfiguration<MeasurementConfig>();
  auto measurement_images_nb = std::max<unsigned int>(1, measurement_config.getMeasurementImages().size());

  std::map<std::string, unsigned> pos_in_group;

  for (int i = 0; i < measurement_images_nb; ++i) {
    auto& group = measurement_config.getGroupForImage(i);
    unsigned pos = pos_in_group[group->getName()]++;
    m_auto_names.emplace_back(std::make_pair(group->getName() + "_" + std::to_string(pos), i));
  }
}

void AutoPhotometryTaskFactory::registerPropertyInstances(SExtractor::OutputRegistry &output_registry) {
  output_registry.registerPropertyInstances<AutoPhotometry>(m_auto_names);
}

std::shared_ptr<Task> AutoPhotometryTaskFactory::createTask(const PropertyId &property_id) const {
  if (property_id.getTypeId() == typeid(AutoPhotometry)) {
    return std::make_shared<AutoPhotometryTask>(property_id.getIndex(), m_magnitude_zero_point, m_kron_factor,
                                                m_kron_minrad, m_symmetry_usage);
  } else if (property_id == PropertyId::create<AutoPhotometryFlag>()) {
    return std::make_shared<AutoPhotometryFlagTask>(m_kron_factor, m_kron_minrad);
  } else {
    return nullptr;
  }
}

}
