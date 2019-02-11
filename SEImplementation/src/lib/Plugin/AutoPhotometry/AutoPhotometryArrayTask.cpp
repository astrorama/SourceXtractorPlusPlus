/*
 * AutoPhotometryArrayTask.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryArray.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryArrayTask.h"


namespace SExtractor {


void AutoPhotometryArrayTask::computeProperties(SExtractor::SourceInterface &source) const {
  std::vector<AutoPhotometry> measurements;
  for (auto img : m_images) {
    measurements.emplace_back(source.getProperty<AutoPhotometry>(img));
  }
  source.setProperty<AutoPhotometryArray>(measurements);
}


} // end SExtractor
