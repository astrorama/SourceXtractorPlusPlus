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

namespace SExtractor {

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
  GroupingConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

  Algorithm getAlgorithmOption() const {
    return m_selected_algorithm;
  }

  double getMoffatThreshold() const {
    return m_moffat_threshold;
  }

private:
  Algorithm m_selected_algorithm;
  double m_moffat_threshold;

}; /* End of GroupingConfig class */

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_CONFIGURATION_GROUPINGCONFIG_H_ */
