/*
 * @file ApertureConfig.cpp
 * @author Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Configuration/ApertureConfig.h"
#include "SEImplementation/Configuration/PythonConfig.h"

namespace SExtractor {

ApertureConfig::ApertureConfig(long manager_id): Configuration(manager_id) {
  declareDependency<PythonConfig>();
}

void ApertureConfig::initialize(const Euclid::Configuration::Configuration::UserValues& /*args*/) {
  auto &py = getDependency<PythonConfig>().getInterpreter();

  std::map<int, int> output;

  auto apertures = py.getApertures();
  for (auto &a : apertures) {
    m_apertures.emplace(a.first, a.second.apertures);
    output.emplace(a.second.id, a.first);
  }

  auto output_columns = py.getModelFittingOutputColumns();
  for (auto &oc : output_columns) {
    for (auto &oi : oc.second) {
      auto ai = output.find(oi);
      if (ai != output.end()) {
        m_output_images.push_back(ai->second);
      }
    }
  }
}

std::vector<float> ApertureConfig::getAperturesForImage(int image_id) const {
  auto i = m_apertures.find(image_id);
  if (i == m_apertures.end()) {
    return {};
  }
  return i->second;
}

const std::map<int, std::vector<float>>& ApertureConfig::getApertures() const {
  return m_apertures;
}

} // end SExtractor
