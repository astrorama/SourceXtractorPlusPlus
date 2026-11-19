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
#ifndef _SEIMPLEMENTATION_PLUGIN_SPREADMODELALT_SPREADMODELALTTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_SPREADMODELALT_SPREADMODELALTTASKFACTORY_H_

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "SEFramework/Task/TaskFactory.h"

namespace SourceXtractor {

class SpreadModelAltTaskFactory : public TaskFactory {
public:
  virtual ~SpreadModelAltTaskFactory() = default;

  std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void registerPropertyInstances(OutputRegistry& registry) override;

private:
  std::vector<int> m_measurement_frames;
  unsigned int m_max_iterations = 100;
  std::string m_least_squares_engine = "levmar";
  double m_scale_factor = 1;
  double m_position_prior_sigma = 0.2;
  std::size_t m_max_fit_size = 1000;
  std::map<int, bool> m_should_renormalize;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_SPREADMODELALT_SPREADMODELALTTASKFACTORY_H_ */
