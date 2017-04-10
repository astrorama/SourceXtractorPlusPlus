/**
 * @file SEImplementation/Configuration/SegmentationConfig.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATIONCONFIG_H
#define _SEIMPLEMENTATION_SEGMENTATIONCONFIG_H

#include <memory>

#include "SEFramework/Image/ImageProcessing.h"

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

  /// Constructs a new SegmentationConfig object
  SegmentationConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  Algorithm getAlgorithmOption() const {
    return m_selected_algorithm;
  }

  bool isFilteringEnabled() const {
    return m_filter != nullptr;
  }

  std::shared_ptr<DetectionImageProcessing> getFilter() const {
    return m_filter;
  }

private:
  std::shared_ptr<DetectionImageProcessing> getDefaultFilter() const;
  std::shared_ptr<DetectionImageProcessing> loadFilter(const std::string& filename) const;

  Algorithm m_selected_algorithm;
  bool m_filtering_enabled;
  std::shared_ptr<DetectionImageProcessing> m_filter;

}; /* End of SegmentationConfig class */

} /* namespace SExtractor */


#endif
