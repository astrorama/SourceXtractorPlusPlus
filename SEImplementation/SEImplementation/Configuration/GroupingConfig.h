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
 * GroupingConfig.h
 *
 *  Created on: Jul 3, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_GROUPINGCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_GROUPINGCONFIG_H_

#include <memory>
#include "Configuration/Configuration.h"

namespace SourceXtractor {

/**
 * @class GroupingConfig
 * @brief Used to select a Grouping algorithm.
 */
class GroupingConfig : public Euclid::Configuration::Configuration {
public:

  enum class Algorithm {
    NO_GROUPING,
    OVERLAPPING,
    SPLIT_SOURCES,
    MOFFAT,
  };

  /**
   * @brief Destructor
   */
  virtual ~GroupingConfig() = default;

  /// Constructs a new GroupingConfig object
  explicit GroupingConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

  Algorithm getAlgorithmOption() const {
    return m_selected_algorithm;
  }

  double getMoffatThreshold() const {
    return m_moffat_threshold;
  }

  double getMoffatMaxDistance() const {
    return m_moffat_max_distance;
  }

  unsigned int getHardLimit() const {
    return m_hard_limit;
  }

private:
  Algorithm m_selected_algorithm;
  double m_moffat_threshold;
  double m_moffat_max_distance;
  unsigned int m_hard_limit;

}; /* End of GroupingConfig class */

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_CONFIGURATION_GROUPINGCONFIG_H_ */
