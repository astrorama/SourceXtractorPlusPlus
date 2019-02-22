/*
 * AperturePhotometryArrayTask.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryArray.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryArrayTask.h"

namespace SExtractor {

void AperturePhotometryArrayTask::computeProperties(SExtractor::SourceInterface &source) const {
  std::vector<AperturePhotometry> measurements;
  for (auto i : m_aperture_instances) {
    measurements.emplace_back(source.getProperty<AperturePhotometry>(i));
  }
  source.setIndexedProperty<AperturePhotometryArray>(m_instance, measurements);
}

} // end SExtractor
