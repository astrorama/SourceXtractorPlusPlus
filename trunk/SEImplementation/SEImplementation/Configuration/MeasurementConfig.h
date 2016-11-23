/*
 * MeasurementConfig.h
 *
 *  Created on: Nov 4, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MEASUREMENTCONFIG_H_

#include <boost/property_tree/ptree.hpp>

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

class MeasurementConfig : public Euclid::Configuration::Configuration {
public:

  class AperturePhotometryOptions {
  public:
    enum class AggregateType {
      None,
      Mean
    };

    AperturePhotometryOptions() : m_aggregate_type(AggregateType::None) {}

    void updateOptions(const boost::property_tree::ptree& image_group);

  private:
    AggregateType m_aggregate_type;
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

  private:
    std::set<unsigned int> m_measurement_image_indices;
    AperturePhotometryOptions m_aperture_options;
  };


  /**
   * @brief Destructor
   */
  virtual ~MeasurementConfig() = default;

  /// Constructs a new DetectionImageConfig object
  MeasurementConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  void parseTree();
  void parseImageGroup(const boost::property_tree::ptree& image_group, AperturePhotometryOptions ap_options);
  std::set<unsigned int> parseImageFiles(const boost::property_tree::ptree& image_group);

  const std::vector<std::shared_ptr<MeasurementImage>>& getMeasurementImages() const {
    return m_measurement_images;
  }

  const std::vector<std::shared_ptr<CoordinateSystem>>& getCoordinateSystems() const {
    return m_coordinate_systems;
  }

  const std::vector<std::shared_ptr<ImageGroup>>& getImageGroups() const {
    return m_groups;
  }

  unsigned int addImage(const std::string filename);

private:
  boost::property_tree::ptree m_property_tree;
  std::vector<std::shared_ptr<ImageGroup>> m_groups;
  std::vector<std::shared_ptr<MeasurementImage>> m_measurement_images;
  std::vector<std::shared_ptr<CoordinateSystem>> m_coordinate_systems;
};

}

#endif
