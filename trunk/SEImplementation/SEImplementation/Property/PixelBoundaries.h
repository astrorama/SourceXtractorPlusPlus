/**
 * @file SEImplementation/PixelBoundaries.h
 * @date 05/30/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_PIXELBOUNDARIES_H
#define _SEIMPLEMENTATION_PROPERTY_PIXELBOUNDARIES_H

#include "SEUtils/PixelCoordinate.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SEImplementation {

using SEUtils::PixelCoordinate;
using SEFramework::Property;
using SEFramework::PropertyId;
using SEFramework::Source;
using SEFramework::SourceTask;
using SEFramework::Task;
using SEFramework::TaskFactory;

/**
 * @class PixelBoundaries
 * @brief
 *
 */
class PixelBoundaries : public Property {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundaries() = default;

  PixelBoundaries(int min_x, int min_y, int max_x, int max_y) : m_min(min_x, min_y), m_max(max_x, max_y) {
  }

  PixelCoordinate getMin() const {
    return m_min;
  }

  PixelCoordinate getMax() const {
    return m_max;
  }

private:
  PixelCoordinate m_min, m_max;

}; /* End of PixelBoundaries class */

class PixelBoundariesTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundariesTask() = default;

  virtual void computeProperties(Source& source) const override;


private:

}; /* End of PixelBoundariesTask class */

class PixelBoundariesTaskFactory : public TaskFactory {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelBoundariesTaskFactory() = default;

  PixelBoundariesTaskFactory();

  virtual std::shared_ptr<Task> getTask(PropertyId property_id) override;
  virtual const std::vector<PropertyId> getProducedProperties() override;

private:
  std::shared_ptr<PixelBoundariesTask> m_pixel_boundaries_task;

private:

}; /* End of PixelBoundariesTaskFactory class */

} /* namespace SEImplementation */


#endif
