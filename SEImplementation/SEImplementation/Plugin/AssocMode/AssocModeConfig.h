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


#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODECONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODECONFIG_H_

#include <boost/algorithm/string/split.hpp>

#include "Configuration/Configuration.h"
#include "Table/TableReader.h"

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

  void readTable(const Euclid::Table::Table& table,
      const std::vector<int>& columns, const std::vector<int>& copy_columns);

  AssocMode m_assoc_mode;
  double m_assoc_radius;

  std::vector<CatalogEntry> m_catalog;
};

} /* namespace SourceXtractor */


#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODECONFIG_H_ */
