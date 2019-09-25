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
 * @file MeasurementImageConfig.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTIMAGECONFIG_H
#define _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTIMAGECONFIG_H

#include <vector>
#include <memory>

#include <Configuration/Configuration.h>

#include <SEFramework/Image/Image.h>
#include <SEFramework/CoordinateSystem/CoordinateSystem.h>

#include <SEImplementation/Configuration/WeightImageConfig.h>

namespace SExtractor {

class MeasurementImageConfig : public Euclid::Configuration::Configuration {
  
public:
  
  struct MeasurementImageInfo {
    std::string m_path;
    std::string m_psf_path;

    std::shared_ptr<MeasurementImage> m_measurement_image;
    std::shared_ptr<CoordinateSystem> m_coordinate_system;
    std::shared_ptr<WeightImage> m_weight_image;

    WeightImageConfig::WeightType m_weight_type;

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

