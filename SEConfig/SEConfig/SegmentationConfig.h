/**
 * @file SEConfig/SegmentationConfig.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SECONFIG_SEGMENTATIONCONFIG_H
#define _SECONFIG_SEGMENTATIONCONFIG_H

#include "Configuration/Configuration.h"

namespace SExtractor {

/**
 * @class SegmentationConfig
 * @brief
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
  void initialize(const UserValues& args) override;

  Algorithm getAlgorithmOption() const {
    return m_selected_algorithm;
  }

private:
  Algorithm m_selected_algorithm;

}; /* End of SegmentationConfig class */

} /* namespace SEConfig */


#endif
