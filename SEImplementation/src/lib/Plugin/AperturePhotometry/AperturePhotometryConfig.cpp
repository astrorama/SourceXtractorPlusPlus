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
 * @file AperturePhotometryConfig.cpp
 * @author Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryConfig.h"
#include "SEImplementation/Configuration/PythonConfig.h"

namespace SExtractor {

AperturePhotometryConfig::AperturePhotometryConfig(long manager_id): Configuration(manager_id) {
  declareDependency<PythonConfig>();
}

void AperturePhotometryConfig::initialize(const Euclid::Configuration::Configuration::UserValues& /*args*/) {
  auto& py = getDependency<PythonConfig>().getInterpreter();

  // Used to get the image corresponding to a given aperture ID
  std::map<unsigned, unsigned> ap_id_to_img;

  // These are the apertures we need to compute for each image
  auto apertures = py.getApertures();
  for (auto& a : apertures) {
    m_apertures.emplace(a.first, a.second.apertures);
    ap_id_to_img.emplace(a.second.id, a.first);
  }

  // And these are the output columns. If several apertures are defined together, they go out together
  auto output_columns = py.getApertureOutputColumns();
  for (auto& oc : output_columns) {
    auto col_name = oc.first;
    for (auto& ap_id : oc.second) {
      auto img_id_iter = ap_id_to_img.find(ap_id);
      if (img_id_iter != ap_id_to_img.end()) {
        auto img_id = img_id_iter->second;
        // Which images ID correspond to an output column
        m_output_images[col_name].push_back(img_id);
      }
    }
  }
}

std::vector<float> AperturePhotometryConfig::getAperturesForImage(unsigned image_id) const {
  auto i = m_apertures.find(image_id);
  if (i == m_apertures.end()) {
    return {};
  }
  return i->second;
}

const std::map<unsigned, std::vector<float>>& AperturePhotometryConfig::getApertures() const {
  return m_apertures;
}

} // end SExtractor
