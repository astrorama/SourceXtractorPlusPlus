/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltConfig.h"
#include "SEImplementation/Plugin/SpreadModelAlt/JointSpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/JointSpreadModelAltTask.h"
#include "SEImplementation/Plugin/SpreadModelAlt/CombinedSpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/CombinedSpreadModelAltTask.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltTask.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltTaskFactory.h"
#include "SEImplementation/Configuration/MeasurementImageConfig.h"
#include "SEImplementation/Configuration/LegacyModelFittingConfig.h"
#include "SEImplementation/Configuration/SamplingConfig.h"


namespace SourceXtractor {

std::shared_ptr<Task> SpreadModelAltTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == typeid(SpreadModelAlt) && m_should_renormalize.count(property_id.getIndex()) != 0) {
    return std::make_shared<SpreadModelAltTask>(property_id.getIndex(), m_measurement_frames,
        m_max_iterations, m_least_squares_engine,
        m_scale_factor, m_max_fit_size, m_should_renormalize);
  }

  if (property_id == PropertyId::create<CombinedSpreadModelAlt>()) {
    return std::make_shared<CombinedSpreadModelAltTask>(m_measurement_frames);
  }

  if (property_id == PropertyId::create<JointSpreadModelAlt>()) {
    return std::make_shared<JointSpreadModelAltTask>(m_measurement_frames, m_max_iterations,
        m_least_squares_engine, m_scale_factor, m_max_fit_size, m_should_renormalize, m_position_prior_sigma);
  }

  return nullptr;
}

void SpreadModelAltTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<MeasurementImageConfig>();
  manager.registerConfiguration<LegacyModelFittingConfig>();
  manager.registerConfiguration<SamplingConfig>();
  manager.registerConfiguration<SpreadModelAltConfig>();
}

void SpreadModelAltTaskFactory::registerPropertyInstances(OutputRegistry& registry) {
  // Column suffixes follow configuration order; property indices retain actual
  // frame IDs, which need not be consecutive.
  std::vector<std::pair<std::string, unsigned int>> instances;
  for (std::size_t i = 0; i < m_measurement_frames.size(); ++i) {
    instances.emplace_back(std::to_string(i), m_measurement_frames[i]);
  }
  registry.registerPropertyInstances<SpreadModelAlt>(instances);
}

void SpreadModelAltTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& measurement_config = manager.getConfiguration<MeasurementImageConfig>();
  for (auto& image_info : measurement_config.getImageInfos()) {
    m_measurement_frames.push_back(image_info.m_id);
    m_should_renormalize[image_info.m_id] = image_info.m_psf_renormalize;
  }

  auto& model_fitting_config = manager.getConfiguration<LegacyModelFittingConfig>();
  m_max_iterations = model_fitting_config.getMaxIterations();
  m_least_squares_engine = model_fitting_config.getLeastSquaresEngine();

  m_position_prior_sigma = manager.getConfiguration<SpreadModelAltConfig>().getPositionPriorSigma();

  auto& sampling_config = manager.getConfiguration<SamplingConfig>();
  m_scale_factor = sampling_config.getScaleFactor();
  m_max_fit_size = sampling_config.getMaxFitSize();
}

}
