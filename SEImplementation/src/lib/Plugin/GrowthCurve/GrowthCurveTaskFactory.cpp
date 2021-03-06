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

#include <SEImplementation/Configuration/MeasurementImageConfig.h>
#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurve.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveResampled.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveTask.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveResampledTask.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveTaskFactory.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveConfig.h"

namespace SourceXtractor {

std::shared_ptr<Task> GrowthCurveTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == typeid(GrowthCurve)) {
    return std::make_shared<GrowthCurveTask>(property_id.getIndex(), m_use_symmetry);
  }
  else if (property_id.getTypeId() == typeid(GrowthCurveResampled)) {
    return std::make_shared<GrowthCurveResampledTask>(m_images, m_nsamples);
  }
  return nullptr;
}

void GrowthCurveTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<GrowthCurveConfig>();
  manager.registerConfiguration<WeightImageConfig>();
  manager.registerConfiguration<MeasurementImageConfig>();
}

void GrowthCurveTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  m_nsamples = manager.getConfiguration<GrowthCurveConfig>().m_nsamples;
  m_use_symmetry = manager.getConfiguration<WeightImageConfig>().symmetryUsage();

  auto& measurement_config = manager.getConfiguration<MeasurementImageConfig>();
  const auto& image_infos = measurement_config.getImageInfos();

  std::map<std::string, unsigned> pos_in_group;

  for (size_t i = 0; i < image_infos.size(); ++i) {
    m_images.push_back(image_infos[i].m_id);
  }
}

} // end of namespace SourceXtractor
