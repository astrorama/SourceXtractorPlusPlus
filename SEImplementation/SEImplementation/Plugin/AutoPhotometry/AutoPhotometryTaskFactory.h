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
 * AutoPhotometryTaskFactory.h
 *
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_

#include "SEUtils/Types.h"
#include "SEFramework/Task/TaskFactory.h"


namespace SourceXtractor {

class AutoPhotometryTaskFactory : public TaskFactory {
public:
  AutoPhotometryTaskFactory();

  /// Destructor
  virtual ~AutoPhotometryTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  void registerPropertyInstances(OutputRegistry &) override;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

private:
  SeFloat m_magnitude_zero_point;
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  bool    m_symmetry_usage;
  std::vector<std::pair<std::string, unsigned int>> m_auto_names;
  std::vector<unsigned> m_images;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_ */
