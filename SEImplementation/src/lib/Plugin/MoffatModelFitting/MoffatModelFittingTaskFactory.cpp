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
 * MoffatModelFittingTaskFactory.cpp
 *
 *  Created on: May 17, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelEvaluator.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingTaskFactory.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingTask.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelEvaluatorTask.h"
#include "SEImplementation/Configuration/LegacyModelFittingConfig.h"

namespace SourceXtractor {

std::shared_ptr<Task> MoffatModelFittingTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<MoffatModelFitting>()) {
    return std::make_shared<MoffatModelFittingTask>(m_least_squares_engine, m_max_iterations);
  } else if (property_id == PropertyId::create<MoffatModelEvaluator>()) {
    return std::make_shared<MoffatModelEvaluatorTask>();
  } else {
    return nullptr;
  }
}

void MoffatModelFittingTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<LegacyModelFittingConfig>();
}

void MoffatModelFittingTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& model_fitting_config = manager.getConfiguration<LegacyModelFittingConfig>();
  m_max_iterations = model_fitting_config.getMaxIterations();
  m_least_squares_engine = model_fitting_config.getLeastSquaresEngine();
}

}

