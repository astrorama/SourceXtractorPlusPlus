/*
 * AssocModeTask.h
 *
 *  Created on: Mar 5, 2021
 *      Author: mschefer
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
  using CatalogCompare = std::function<bool(const CatalogEntry&, const CatalogEntry&)>;

  using GetAssocResult = std::function<AssocModeConfig::CatalogEntry(const std::vector<AssocModeConfig::CatalogEntry>&)>;

  /// Destructor
  virtual ~AssocModeTask() = default;

  AssocModeTask(const std::vector<AssocModeConfig::CatalogEntry>& catalog, AssocModeConfig::AssocMode assoc_type, double radius);

  virtual void computeProperties(SourceInterface& source) const override;

private:
  std::vector<AssocModeConfig::CatalogEntry> findSourcesWithinRadius(double x, double y, double radius) const;
  AssocModeConfig::CatalogEntry& findBestMatch(std::vector<AssocModeConfig::CatalogEntry> selection, CatalogCompare compare) const;

  std::vector<AssocModeConfig::CatalogEntry> m_catalog;
  AssocModeConfig::AssocMode m_assoc_mode;
  double m_radius;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODETASK_H_ */
