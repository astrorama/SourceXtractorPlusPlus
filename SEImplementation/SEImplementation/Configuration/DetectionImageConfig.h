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
/**
 * @file SEImplementation/Configuration/DetectionImageConfig.h
 * @date 06/06/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_DETECTIONIMAGECONFIG_H
#define _SEIMPLEMENTATION_DETECTIONIMAGECONFIG_H

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageSourceWithMetadata.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SourceXtractor {

/**
 * @class DetectionImageConfig
 * @brief Provides the detection image
 *
 */
class DetectionImageConfig : public Euclid::Configuration::Configuration {
 public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionImageConfig() = default;

  /// Constructs a new DetectionImageConfig object
  explicit DetectionImageConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  
  void initialize(const UserValues& args) override;

  std::string getDetectionImagePath() const;

  std::shared_ptr<DetectionImage> getDetectionImage(size_t index = 0) const;
  std::shared_ptr<CoordinateSystem> getCoordinateSystem(size_t index = 0) const;
  
  double getGain(size_t index = 0) const { return m_extensions.at(index).m_gain; }
  double getSaturation(size_t index = 0) const { return m_extensions.at(index).m_saturation; }
  int getInterpolationGap(size_t index = 0) const { return m_extensions.at(index).m_interpolation_gap; }

  // Note: flux scale is already applied to all values returned,
  // we still need to know what it was to adjust the weight map
  double getOriginalFluxScale(size_t index = 0) const { return m_extensions.at(index).m_flux_scale; }

  // Get the detection image source
  std::shared_ptr<ImageSource> getImageSource(size_t index = 0) {
    return m_extensions.at(index).m_image_source;
  }

  size_t getExtensionsNb() const {
    return m_extensions.size();
  }

private:
  std::string m_detection_image_path;

  struct DetectionImageExtension {
    std::shared_ptr<DetectionImage> m_detection_image;
    std::shared_ptr<CoordinateSystem> m_coordinate_system;
    std::shared_ptr<ImageSource> m_image_source;

    double m_gain {0.0};
    double m_saturation {0.0};
    double m_flux_scale {1.0};

    int m_interpolation_gap {0};
  };

  std::vector<DetectionImageExtension> m_extensions;

}; /* End of DetectionImageConfig class */

} /* namespace SourceXtractor */


#endif
