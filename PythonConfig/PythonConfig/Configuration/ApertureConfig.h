/*
 * @file ApertureConfig.h
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _PYTHON_CONFIG_CONFIGURATION_APERTURECONFIG_H
#define _PYTHON_CONFIG_CONFIGURATION_APERTURECONFIG_H

#include <Configuration/Configuration.h>

namespace SExtractor {

class ApertureConfig: public Euclid::Configuration::Configuration {
public:
  ApertureConfig(long manager_id);

  void initialize(const UserValues& args) override;

  std::vector<float> getAperturesForImage(int image_id) const;

  std::vector<int> getOutputForImages() const {
    return m_output_images;
  }

private:
  // Map the image id to the apertures
  std::map<int, std::vector<float>> m_apertures;
  // List of images for which we write a column
  std::vector<int> m_output_images;
};

}

#endif // _PYTHON_CONFIG_CONFIGURATION_APERTURECONFIG_H
