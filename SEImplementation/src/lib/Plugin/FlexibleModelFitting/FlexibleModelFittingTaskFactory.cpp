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
/*
 * FlexibleModelFittingTaskFactory.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#include <ElementsKernel/Logging.h>
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFitting.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTask.h"
#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingTaskFactory.h"
#include "SEImplementation/Configuration/ModelFittingConfig.h"

namespace SourceXtractor {

static auto logger = Elements::Logging::getLogger("FlexibleModelFitting");

std::shared_ptr<Task> FlexibleModelFittingTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<FlexibleModelFitting>()) {
    return std::make_shared<FlexibleModelFittingTask>(m_least_squares_engine, m_max_iterations,
                                                      m_modified_chi_squared_scale, m_parameters, m_frames, m_priors);
  } else {
    return nullptr;
  }
}

void FlexibleModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ModelFittingConfig>();
}

void FlexibleModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& model_fitting_config = manager.getConfiguration<ModelFittingConfig>();

  for (auto const& i : model_fitting_config.getParameters()) {
    m_parameters.push_back(i.second);
  }

  m_frames = model_fitting_config.getFrames();
  
  for (auto const& p : model_fitting_config.getPriors()) {
    m_priors.push_back(p.second);
  }

  m_least_squares_engine = model_fitting_config.getLeastSquaresEngine();
  m_max_iterations = model_fitting_config.getMaxIterations();
  m_modified_chi_squared_scale = model_fitting_config.getModifiedChiSquaredScale();

  logger.info() << "Using engine " << m_least_squares_engine << " with "
                << m_max_iterations << " maximum number of iterations";

  m_outputs = model_fitting_config.getOutputs();
}

void FlexibleModelFittingTaskFactory::registerPropertyInstances(OutputRegistry& registry) {
  for (auto& p : m_outputs) {
    std::string name = p.first;
    std::vector<int> properties = p.second;
    if (properties.size() == 1) {
      registry.registerColumnConverter<FlexibleModelFitting, double>(name,
          [properties](const FlexibleModelFitting& prop) {
            return prop.getParameterValue(properties[0]);
          }
      );
      registry.registerColumnConverter<FlexibleModelFitting, double>(name+"_err",
          [properties](const FlexibleModelFitting& prop) {
            return prop.getParameterSigma(properties[0]);
          }
      );
    } else {
      registry.registerColumnConverter<FlexibleModelFitting, std::vector<double>>(name,
          [properties](const FlexibleModelFitting& prop) {
            std::vector<double> result;
            for (int id : properties) {
              result.push_back(prop.getParameterValue(id));
            }
            return result;
          }
      );
      registry.registerColumnConverter<FlexibleModelFitting, std::vector<double>>(name+"_err",
          [properties](const FlexibleModelFitting& prop) {
            std::vector<double> result;
            for (int id : properties) {
              result.push_back(prop.getParameterSigma(id));
            }
            return result;
          }
      );
    }
  }
}

}
