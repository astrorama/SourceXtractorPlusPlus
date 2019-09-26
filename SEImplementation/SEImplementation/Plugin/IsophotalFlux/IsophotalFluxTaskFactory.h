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
 * @file SEImplementation/IsophotalFluxTaskFactory.h
 * @date 09/28/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_ISOPHOTALFLUXTASKFACTORY_H
#define _SEIMPLEMENTATION_TASK_ISOPHOTALFLUXTASKFACTORY_H

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

/**
 * @class IsophotalFluxTaskFactory
 * @brief Produces IsophotalFluxTask
 *
 */
class IsophotalFluxTaskFactory : public TaskFactory {
public:
  IsophotalFluxTaskFactory() : m_magnitude_zero_point(0) {}

  /// Destructor
  virtual ~IsophotalFluxTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  SeFloat m_magnitude_zero_point;
};

} /* namespace SExtractor */


#endif
