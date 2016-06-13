/**
 * @file Segmentation/Lutz/Lutz.h
 * @date 05/31/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H
#define _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H

#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Pipeline/Segmentation.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class PixelGroup;

/**
 * @class Lutz
 * @brief
 */
class Lutz  : public Segmentation {

public:

  /**
   * @brief Destructor
   */
  virtual ~Lutz() = default;

  Lutz(std::shared_ptr<TaskRegistry> task_registry) : m_task_registry(task_registry) {}

  virtual void scan(const Image& image) override;


private:
  void publishGroup(PixelGroup& pixel_group);

  std::shared_ptr<TaskRegistry> m_task_registry;

}; /* End of Lutz class */

} /* namespace SEImplementation */


#endif
