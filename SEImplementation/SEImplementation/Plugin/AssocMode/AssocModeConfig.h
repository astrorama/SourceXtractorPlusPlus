/*
 * AssocConfig.h
 *
 *  Created on: Mar 4, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODECONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODECONFIG_H_

#include "Configuration/Configuration.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SourceXtractor {

class AssocModeConfig : public Euclid::Configuration::Configuration {

public:

  enum class AssocMode {
    UNKNOWN,
    FIRST,
    NEAREST,
    MIN,
    MAX,
    MEAN,
    MAG_MEAN,
    SUM,
    MAG_SUM
  };

  enum class AssocFilter {
    ALL,
    MATCHED,
    UNMATCHED
  };

  struct CatalogEntry {
    ImageCoordinate coord;
    double weight;
    std::vector<double> assoc_columns;
  };


  AssocModeConfig(long manager_id);
  virtual ~AssocModeConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

  AssocMode getAssocMode() const {
    return m_assoc_mode;
  }

  double getAssocRadius() const {
    return m_assoc_radius;
  }

  const std::vector<CatalogEntry>& getCatalog() const {
    return m_catalog;
  }

private:
  AssocMode m_assoc_mode;
  double m_assoc_radius;

  std::vector<CatalogEntry> m_catalog;
};

} /* namespace SourceXtractor */


#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODECONFIG_H_ */
