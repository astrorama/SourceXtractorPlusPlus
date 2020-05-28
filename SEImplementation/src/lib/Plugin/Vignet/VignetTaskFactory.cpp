/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file VignetPlugin.cpp
 *
 * @date Jan 23, 2020
 * @author mkuemmel@usm.lmu.de
 */

#include "SEImplementation/Configuration/MeasurementImageConfig.h"
#include "SEImplementation/Plugin/Vignet/VignetTaskFactory.h"
#include "SEImplementation/Plugin/Vignet/VignetConfig.h"
#include "SEImplementation/Plugin/Vignet/Vignet.h"
#include "SEImplementation/Plugin/Vignet/VignetArray.h"
#include "SEImplementation/Plugin/Vignet/VignetSourceTask.h"
#include "SEImplementation/Plugin/Vignet/VignetArraySourceTask.h"

namespace SourceXtractor {

void VignetTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<VignetConfig>();
}

void VignetTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto vignet_config = manager.getConfiguration<VignetConfig>();
  m_vignet_size = vignet_config.getVignetSize();
  m_vignet_default_pixval = vignet_config.getVignetDefaultPixval();

  auto& measurement_config = manager.getConfiguration<MeasurementImageConfig>();
  const auto& image_infos = measurement_config.getImageInfos();

  std::map<std::string, unsigned> pos_in_group;

  for (size_t i = 0; i < image_infos.size(); ++i) {
    m_images.push_back(image_infos[i].m_id);
  }
}

std::shared_ptr<Task> VignetTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == typeid(Vignet)) {
    return std::make_shared<VignetSourceTask>(property_id.getIndex(), m_vignet_size, m_vignet_default_pixval);
  }
  else if (property_id == PropertyId::create<VignetArray>()) {
    return std::make_shared<VignetArraySourceTask>(m_images);
  }
  return nullptr;
}

} // end of namespace SourceXtractor

