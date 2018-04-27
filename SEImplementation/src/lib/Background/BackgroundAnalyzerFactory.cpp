/*
 * BackgroundAnalyzerFactory.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: mschefer
 */


#include "SEImplementation/Background/BackgroundAnalyzerFactory.h"

#include "SEImplementation/Background/SimpleBackgroundAnalyzer.h"
#include "SEImplementation/Background/SE2BackgroundLevelAnalyzer.h"

namespace SExtractor {

std::shared_ptr<BackgroundAnalyzer> BackgroundAnalyzerFactory::createBackgroundAnalyzer() const {

  // make a SE2 background if cell size and smoothing box are given
  if (m_cell_size.size() > 0 && m_smoothing_box.size() > 0){
    auto background_level =  std::make_shared<SE2BackgroundLevelAnalyzer>(m_cell_size, m_smoothing_box);
    return background_level;
  }
  else {
    // make a simple background
    auto background_level =  std::make_shared<SimpleBackgroundAnalyzer>();
    return background_level;
  }
}

void BackgroundAnalyzerFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<SE2BackgroundConfig>();
}

void BackgroundAnalyzerFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto se2background_config = manager.getConfiguration<SE2BackgroundConfig>();
  m_cell_size = se2background_config.getCellSize();
  m_smoothing_box = se2background_config.getSmoothingBox();
}

}
