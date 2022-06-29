/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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


#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASKFACTORY_H_

#include "AssocModeConfig.h"
#include "SEFramework/Task/TaskFactory.h"


namespace SourceXtractor {

class AssocModeTaskFactory : public TaskFactory {
public:
  AssocModeTaskFactory();

  /// Destructor
  virtual ~AssocModeTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  // TaskFactory implementation
  std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

private:
  std::vector<std::pair<std::string, unsigned int>> m_auto_names;

  AssocModeConfig::AssocMode m_assoc_mode;
  double m_assoc_radius;
  std::vector<std::vector<AssocModeConfig::CatalogEntry>> m_catalogs;
};

}




#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASKFACTORY_H_ */
