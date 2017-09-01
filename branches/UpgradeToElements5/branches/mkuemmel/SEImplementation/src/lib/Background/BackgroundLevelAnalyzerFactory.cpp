/*
 * BackgroundAnalyzerFactory.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: mschefer
 */


#include "SEImplementation/Background/BackgroundLevelAnalyzerFactory.h"

#include "SEImplementation/Background/SimpleBackgroundLevelAnalyzer.h"

namespace SExtractor {

std::shared_ptr<BackgroundAnalyzer> BackgroundLevelAnalyzerFactory::createBackgroundAnalyzer() const {
  return std::make_shared<SimpleBackgroundLevelAnalyzer>();
}

void BackgroundLevelAnalyzerFactory::configure(Euclid::Configuration::ConfigManager&) {

}


void BackgroundLevelAnalyzerFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager&) const {

}

}
