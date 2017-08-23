/*
 * BackgroundAnalyzerFactory.h
 *
 *  Created on: Mar 29, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_
#define _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Background/BackgroundAnalyzer.h"

namespace SExtractor {

class BackgroundLevelAnalyzerFactory  : public Configurable {
public:
  /// Destructor
  virtual ~BackgroundLevelAnalyzerFactory() = default;

  std::shared_ptr<BackgroundAnalyzer> createBackgroundAnalyzer() const;

  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:

};

}



#endif /* _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_ */
