/*
 * BackgroundRMSAnalyzerFactory.h
 *
 *  Created on: Apr 25, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDRMSANALYZERFACTORY_H_
#define _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDRMSANALYZERFACTORY_H_

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Background/BackgroundAnalyzer.h"

namespace SExtractor {

class BackgroundRMSAnalyzerFactory  : public Configurable {
public:
  /// Destructor
  virtual ~BackgroundRMSAnalyzerFactory() = default;

  std::shared_ptr<BackgroundAnalyzer> createBackgroundAnalyzer() const;

  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:

};

}


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_BACKGROUND_BACKGROUNDRMSANALYZERFACTORY_H_ */
