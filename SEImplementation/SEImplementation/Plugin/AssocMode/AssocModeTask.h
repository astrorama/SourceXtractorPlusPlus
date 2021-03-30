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


#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASK_H_


#include "AssocModeConfig.h"
#include "SEUtils/Types.h"
#include "SEFramework/Task/SourceTask.h"


namespace SourceXtractor {

class AssocModeTask : public SourceTask {
public:
  using CatalogEntry = AssocModeConfig::CatalogEntry;
  using GetAssocResult = std::function<AssocModeConfig::CatalogEntry(const std::vector<AssocModeConfig::CatalogEntry>&)>;

  /// Destructor
  virtual ~AssocModeTask() = default;

  AssocModeTask(const std::vector<AssocModeConfig::CatalogEntry>& catalog, AssocModeConfig::AssocMode assoc_type, double radius);

  virtual void computeProperties(SourceInterface& source) const override;

private:
  std::vector<AssocModeConfig::CatalogEntry> findSourcesWithinRadius(double x, double y, double radius) const;

  std::vector<AssocModeConfig::CatalogEntry> m_catalog;
  AssocModeConfig::AssocMode m_assoc_mode;
  double m_radius;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASK_H_ */
