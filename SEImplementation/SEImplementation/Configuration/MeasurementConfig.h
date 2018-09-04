/*
 * MeasurementConfig.h
 *
 *  Created on: Nov 4, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTCONFIG_H_

//#include <boost/property_tree/ptree.hpp>

#include <yaml-cpp/yaml.h>

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

#include "SEImplementation/Image/ImagePsf.h"

namespace SExtractor {

class MeasurementConfig : public Euclid::Configuration::Configuration {
public:

  class AperturePhotometryOptions {
  public:
    AperturePhotometryOptions() {}

    void updateOptions(const YAML::Node& image_group);

    std::vector<double> getApertureSizes() const {
      return m_aperture_sizes;
    }

  private:
    std::vector<double> m_aperture_sizes;
  };

  class ImageGroup {
  public:
    void addImages(std::set<unsigned int> images) {
      m_measurement_image_indices.insert(images.begin(), images.end());
    }

    const std::set<unsigned int>& getMeasurementImageIndices() const {
      return m_measurement_image_indices;
    }

    const AperturePhotometryOptions& getAperturePhotometryOptions() const {
      return m_aperture_options;
    }

    void setAperturePhotometryOptions(AperturePhotometryOptions aperture_options) {
      m_aperture_options = aperture_options;
    }

    void setName(const std::string name) {
      m_name = name;
    }

    std::string getName() const;

  private:
    std::set<unsigned int> m_measurement_image_indices;
    AperturePhotometryOptions m_aperture_options;
    std::string m_name;
  };


  /**
   * @brief Destructor
   */
  virtual ~MeasurementConfig() = default;

  /// Constructs a new MeasurementConfig object
  MeasurementConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  void parseTree();
  void parseMeasurementsGroup(const YAML::Node& image_group, AperturePhotometryOptions ap_options);
  std::set<unsigned int> parseImageFiles(const YAML::Node& image_group);

  const std::vector<std::shared_ptr<MeasurementImage>>& getMeasurementImages() const {
    return m_measurement_images;
  }

  const std::vector<std::shared_ptr<CoordinateSystem>>& getCoordinateSystems() const {
    return m_coordinate_systems;
  }

  const std::vector<std::shared_ptr<WeightImage>>& getWeightImages() const {
    return m_weight_images;
  }

  const std::vector<std::string> getPsfsPaths() const {
    return m_psfs_paths;
  }

  const std::vector<bool>& getAbsoluteWeights() const {
    return m_absolute_weights;
  }

  const std::vector<std::shared_ptr<ImageGroup>>& getImageGroups() const {
    return m_groups;
  }

  const std::vector<SeFloat>& getGains() const {
    return m_gains;
  }

  const std::vector<MeasurementImage::PixelType>& getSaturationLevels() const {
    return m_saturation_levels;
  }

  unsigned int addImage(const std::string filename, const std::string weight_filename, const std::string psf_filename);


private:
  std::vector<std::string> getFilenamesFromPath(const std::string& path) const;

  //boost::property_tree::ptree m_property_tree;
  YAML::Node m_yaml_config;

  std::vector<std::shared_ptr<ImageGroup>> m_groups;
  std::vector<std::shared_ptr<MeasurementImage>> m_measurement_images;
  std::vector<std::shared_ptr<CoordinateSystem>> m_coordinate_systems;
  std::vector<std::shared_ptr<WeightImage>> m_weight_images;
  std::vector<std::string> m_psfs_paths;
  std::vector<bool> m_absolute_weights;
  std::vector<SeFloat> m_gains;
  std::vector<MeasurementImage::PixelType> m_saturation_levels;

  bool m_override_gain;
  double m_gain;

  bool m_override_saturation;
  double m_saturation;

  bool m_override_flux_scale;
  double m_flux_scale;

  std::map<std::string, unsigned int> m_loaded_images;
};

}

#endif
