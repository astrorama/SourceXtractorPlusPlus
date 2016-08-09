/**
 * @file SEImplementation/Configuration/DetectionImageConfig.h
 * @date 06/06/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_DETECTIONIMAGECONFIG_H
#define _SEIMPLEMENTATION_DETECTIONIMAGECONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class DetectionImageConfig
 * @brief Provides the detection image
 *
 */
class DetectionImageConfig : public Euclid::Configuration::Configuration {

 public:
   
   /// An action which gets a detection image, wraps it with a decorator and
   /// returns the result
   using DecorateImageAction = std::function<std::shared_ptr<DetectionImage>(std::shared_ptr<DetectionImage>)>;

  /**
   * @brief Destructor
   */
  virtual ~DetectionImageConfig() = default;

  /// Constructs a new DetectionImageConfig object
  DetectionImageConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  
  void initialize(const UserValues& args) override;
  
  void postInitialize(const UserValues& args) override;
  
  void addDecorateImageAction(DecorateImageAction action);
  
  std::shared_ptr<DetectionImage> getOriginalImage() const;
  
  std::shared_ptr<DetectionImage> getDetectionImage() const;
  
private:
  
  std::vector<DecorateImageAction> m_decorate_action_list;
  std::shared_ptr<DetectionImage> m_original_image;
  std::shared_ptr<DetectionImage> m_detection_image;

}; /* End of DetectionImageConfig class */

} /* namespace SExtractor */


#endif
