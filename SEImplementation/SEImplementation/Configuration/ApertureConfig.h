/*
 * @file ApertureConfig.h
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_APERTURECONFIG_H
#define _SEIMPLEMENTATION_CONFIGURATION_APERTURECONFIG_H

#include <Configuration/Configuration.h>

namespace SExtractor {

class ApertureConfig: public Euclid::Configuration::Configuration {
public:
  ApertureConfig(long manager_id);

  void initialize(const UserValues& args) override;

  std::vector<float> getAperturesForImage(int image_id) const;

  const std::map<int, std::vector<float>>& getApertures() const;

  std::map<std::string, std::vector<int>> getImagesToOutput() const {
    return m_output_images;
  }

private:
  // Map the image id to the apertures
  std::map<int, std::vector<float>> m_apertures;
  // List of images for which we write a column
  std::map<std::string, std::vector<int>> m_output_images;
};

}

#endif // _SEIMPLEMENTATION_CONFIGURATION_APERTURECONFIG_H
