/*
 * BackgroundAnalyzerFactory.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: mschefer
 */


#include "SEImplementation/Background/SimpleBackgroundAnalyzer.h"
#include "SEImplementation/Background/BackgroundAnalyzerFactory.h"

namespace SExtractor {

std::shared_ptr<BackgroundAnalyzer> BackgroundAnalyzerFactory::createBackgroundAnalyzer() const {
  return std::make_shared<SimpleBackgroundAnalyzer>();
}

void BackgroundAnalyzerFactory::configure(Euclid::Configuration::ConfigManager&) {

}


void BackgroundAnalyzerFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager&) const {

}

}
