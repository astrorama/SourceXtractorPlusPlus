/**
 * @file SEImplementation/Configuration/SegmentationConfig.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATIONCONFIG_H
#define _SEIMPLEMENTATION_SEGMENTATIONCONFIG_H

#include "Configuration/Configuration.h"

namespace SExtractor {

/**
 * @class SegmentationConfig
 * @brief Used to select a Segmentation algorithm.
 *
 */
class SegmentationConfig : public Euclid::Configuration::Configuration {
public:

  enum class Algorithm {
    UNKNOWN,
    LUTZ
  };

  /**
   * @brief Destructor
   */
  virtual ~SegmentationConfig() = default;

  /// Constructs a new DetectionImageConfig object
  SegmentationConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  Algorithm getAlgorithmOption() const {
    return m_selected_algorithm;
  }

  bool isFilteringEnabled() const {
    return m_filtering_enabled;
  }

private:
  Algorithm m_selected_algorithm;
  bool m_filtering_enabled;

}; /* End of SegmentationConfig class */

} /* namespace SExtractor */


#endif
