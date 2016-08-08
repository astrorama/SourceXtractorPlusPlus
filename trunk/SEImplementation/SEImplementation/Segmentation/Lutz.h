/**
 * @file Segmentation/Lutz/Lutz.h
 * @date 05/31/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H
#define _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Pipeline/Segmentation.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class PixelGroup;

/**
 * @class Lutz
 * @brief Implements a Segmentation based on the Lutz algorithm
 */
class Lutz : public Segmentation {

public:

  /**
   * @brief Destructor
   */
  virtual ~Lutz() = default;

  Lutz(std::shared_ptr<TaskProvider> task_provider) : m_task_provider(task_provider) {}

  virtual void scan(const DetectionImage& image) override;


private:
  void publishGroup(PixelGroup& pixel_group);

  std::shared_ptr<TaskProvider> m_task_provider;

}; /* End of Lutz class */

} /* namespace SExtractor */


#endif
