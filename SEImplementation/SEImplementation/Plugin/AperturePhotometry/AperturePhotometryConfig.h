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
 * @file AperturePhotometryConfig.h
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYCONFIG_H
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYCONFIG_H

#include <Configuration/Configuration.h>

namespace SExtractor {

class AperturePhotometryConfig: public Euclid::Configuration::Configuration {
public:
  AperturePhotometryConfig(long manager_id);

  void initialize(const UserValues& args) override;

  std::vector<float> getAperturesForImage(unsigned image_id) const;

  const std::map<unsigned, std::vector<float>>& getApertures() const;

  std::map<std::string, std::vector<unsigned>> getImagesToOutput() const {
    return m_output_images;
  }

private:
  // Map the image id to the apertures
  std::map<unsigned, std::vector<float>> m_apertures;
  // List of images for which we write a column
  std::map<std::string, std::vector<unsigned>> m_output_images;
};

}

#endif // _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYCONFIG_H
