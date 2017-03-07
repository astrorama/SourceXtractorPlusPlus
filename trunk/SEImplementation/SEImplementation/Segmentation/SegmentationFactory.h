/**
 * @file SEImplementation/SegmentationFactory.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATIONFACTORY_H
#define _SEIMPLEMENTATION_SEGMENTATIONFACTORY_H

#include "SEImplementation/Configuration/SegmentationConfig.h"

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Pipeline/Segmentation.h"

namespace SExtractor {

/**
 * @class SegmentationFactory
 * @brief The SegmentationFactory will provide a Segmentation implementation based on the current configuration
 *
 */
class SegmentationFactory : public Configurable {

public:

  /// Destructor
  virtual ~SegmentationFactory() = default;

  /// Constructor
  SegmentationFactory(std::shared_ptr<TaskProvider> task_provider);

  std::shared_ptr<Segmentation> createSegmentation(SeFloat background_value, SeFloat threshold) const;
  
  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:
  bool m_background_absolute;
  SeFloat m_background_value;
  bool m_threshold_absolute;
  SeFloat m_threshold_value;
  bool m_filtering_enabled;

  SegmentationConfig::Algorithm m_algorithm;
  std::shared_ptr<TaskProvider> m_task_provider;

}; /* End of SegmentationFactory class */

} /* namespace SExtractor */


#endif
