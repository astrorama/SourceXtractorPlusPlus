/* 
 * @file MeasurementImageConfig.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTIMAGECONFIG_H
#define _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTIMAGECONFIG_H

#include <vector>
#include <memory>
#include <SEFramework/Image/Image.h>
#include <SEFramework/CoordinateSystem/CoordinateSystem.h>
#include <Configuration/Configuration.h>

namespace SExtractor {

class MeasurementImageConfig : public Euclid::Configuration::Configuration {
  
public:
  
  struct MeasurementImageInfo {
    std::string m_path;
    std::string m_psf_path;

    std::shared_ptr<MeasurementImage> m_measurement_image;
    std::shared_ptr<CoordinateSystem> m_coordinate_system;
    std::shared_ptr<WeightImage> m_weight_image;

    bool m_absolute_weight;
    WeightImage::PixelType m_weight_threshold;
    SeFloat m_gain;
    MeasurementImage::PixelType m_saturation_level;

    bool m_is_background_constant;
    MeasurementImage::PixelType m_constant_background_value;

    int m_id;
  };

  MeasurementImageConfig(long manager_id);
  
  void initialize(const UserValues& args) override;

  const std::vector<MeasurementImageInfo>& getImageInfos() const {
    return m_image_infos;
  }

private:

  std::vector<MeasurementImageInfo> m_image_infos;
};

}

#endif // _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTIMAGECONFIG_H

