/*
 * BackgroundRMSAnalyserFactory.cpp
 *
 *  Created on: Apr 25, 2017
 *      Author: mschefer
 */



#include "SEImplementation/Background/BackgroundRMSAnalyzerFactory.h"

#include "SEImplementation/Background/SimpleBackgroundRMSAnalyzer.h"

namespace SExtractor {

std::shared_ptr<BackgroundAnalyzer> BackgroundRMSAnalyzerFactory::createBackgroundAnalyzer() const {
  return std::make_shared<SimpleBackgroundRMSAnalyzer>();
}

void BackgroundRMSAnalyzerFactory::configure(Euclid::Configuration::ConfigManager&) {

}


void BackgroundRMSAnalyzerFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager&) const {

}

}


