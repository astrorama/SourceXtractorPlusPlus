/*
 * BackgroundAnalyzerFactory.h
 *
 *  Created on: Mar 29, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_
#define _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_

#include "SEImplementation/Configuration/SE2BackgroundConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Background/BackgroundAnalyzer.h"

namespace SExtractor {

class BackgroundAnalyzerFactory  : public Configurable {
public:
  /// Destructor
  virtual ~BackgroundAnalyzerFactory() = default;

  std::shared_ptr<BackgroundAnalyzer> createBackgroundAnalyzer() const;
  std::shared_ptr<BackgroundAnalyzer> createBackgroundAnalyzer(WeightImageConfig::WeightType weight_type) const;

  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:
  std::string m_cell_size;
  std::string m_smoothing_box;
  WeightImageConfig::WeightType m_weight_type;
};

}



#endif /* _SEIMPLEMENTATION_BACKGROUND_BACKGROUNDANALYZERFACTORY_H_ */
