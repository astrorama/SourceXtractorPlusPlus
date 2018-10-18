/* 
 * @file MeasurementImageConfig.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHON_CONFIG_CONFIGURATION_MEASUREMENTIMAGECONFIG_H
#define _PYTHON_CONFIG_CONFIGURATION_MEASUREMENTIMAGECONFIG_H

#include <vector>
#include <memory>
#include <SEFramework/Image/Image.h>
#include <SEFramework/CoordinateSystem/CoordinateSystem.h>
#include <Configuration/Configuration.h>

namespace SExtractor {

class MeasurementImageConfig : public Euclid::Configuration::Configuration {
  
public:
  
  MeasurementImageConfig(long manager_id);
  
  void initialize(const UserValues& args) override;

  const std::vector<std::shared_ptr<MeasurementImage>>& getMeasurementImages() const;

  const std::vector<std::shared_ptr<CoordinateSystem>>& getCoordinateSystems() const;

  const std::vector<std::shared_ptr<WeightImage>>& getWeightImages() const;

  const std::vector<bool>& getAbsoluteWeights() const;
  
  const std::vector<WeightImage::PixelType>& getWeightThresholds() const;

  const std::vector<std::string> getPsfsPaths() const;

  const std::vector<SeFloat>& getGains() const;

  const std::vector<MeasurementImage::PixelType>& getSaturationLevels() const;
  
private:
  
  std::vector<std::shared_ptr<MeasurementImage>> m_measurement_images;
  std::vector<std::shared_ptr<CoordinateSystem>> m_coordinate_systems;
  std::vector<std::shared_ptr<WeightImage>> m_weight_images;
  std::vector<bool> m_absolute_weights;
  std::vector<WeightImage::PixelType> m_weight_thresholds;
  std::vector<std::string> m_psfs_paths;
  std::vector<SeFloat> m_gains;
  std::vector<MeasurementImage::PixelType> m_saturation_levels;
  
};

}

#endif // _PYTHON_CONFIG_CONFIGURATION_MEASUREMENTIMAGECONFIG_H

