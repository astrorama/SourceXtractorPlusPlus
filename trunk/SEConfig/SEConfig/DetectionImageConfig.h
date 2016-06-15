/**
 * @file SEConfig/DetectionImageConfig.h
 * @date 06/06/16
 * @author mschefer
 */

#ifndef _SECONFIG_DETECTIONIMAGECONFIG_H
#define _SECONFIG_DETECTIONIMAGECONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class DetectionImageConfig
 * @brief
 *
 */
class DetectionImageConfig : public Euclid::Configuration::Configuration {

 public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionImageConfig() = default;

  /// Constructs a new DetectionImageConfig object
  DetectionImageConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

  std::shared_ptr<DetectionImage> getDetectionImage() const {
    return m_detection_image;
  }

private:
  std::shared_ptr<DetectionImage> m_detection_image;

}; /* End of DetectionImageConfig class */

} /* namespace SEConfig */


#endif
