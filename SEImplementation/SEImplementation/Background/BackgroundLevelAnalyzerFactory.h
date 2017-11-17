/*
 * BackgroundAnalyzerFactory.h
 *
 *  Created on: Mar 29, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_
#define _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_

#include "SEImplementation/Configuration/SE2BackgroundConfig.h"

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
  std::string m_cell_size;
  std::string m_smoothing_box;

};

}



#endif /* _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_ */
