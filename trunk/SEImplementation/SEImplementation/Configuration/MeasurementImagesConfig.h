/*
 * MeasurementImagesConfig.h
 *
 *  Created on: Nov 2, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTIMAGESCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTIMAGESCONFIG_H_

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class MeasurementImagesConfig : public Euclid::Configuration::Configuration {
 public:

  /**
   * @brief Destructor
   */
  virtual ~MeasurementImagesConfig() = default;

  /// Constructs a new DetectionImageConfig object
  MeasurementImagesConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  const std::vector<std::shared_ptr<MeasurementImage>>& getMeasurementImages() const {
    return m_measurement_images;
  }

private:
  std::vector<std::shared_ptr<MeasurementImage>> m_measurement_images;
};

}

#endif /* _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTIMAGESCONFIG_H_ */
