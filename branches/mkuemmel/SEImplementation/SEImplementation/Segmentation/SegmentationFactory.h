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

  std::shared_ptr<Segmentation> createSegmentation() const;
  
  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:
  std::shared_ptr<DetectionImageProcessing> m_filter;

  SegmentationConfig::Algorithm m_algorithm;
  std::shared_ptr<TaskProvider> m_task_provider;

}; /* End of SegmentationFactory class */

} /* namespace SExtractor */


#endif
