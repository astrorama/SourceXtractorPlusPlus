/**
 * @file SEImplementation/PixelCentroid.h
 * @date 05/27/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_PIXELCENTROID_H
#define _SEIMPLEMENTATION_PROPERTY_PIXELCENTROID_H

#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Task/TaskFactory.h"

namespace SEImplementation {

using SEFramework::Image;
using SEFramework::Property;
using SEFramework::PropertyId;
using SEFramework::Source;
using SEFramework::SourceTask;
using SEFramework::Task;
using SEFramework::TaskFactory;

/**
 * @class PixelCentroid
 * @brief
 *
 */
class PixelCentroid : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~PixelCentroid() = default;

  PixelCentroid(double centroid_x, double centroid_y) : m_centroid_x(centroid_x), m_centroid_y(centroid_y) {}

  double getCentroidX() const {
    return m_centroid_x;
  }

  double getCentroidY() const {
    return m_centroid_y;
  }

private:
  double m_centroid_x, m_centroid_y;

}; /* End of PixelCentroid class */


/**
 * @class PixelCentroid
 * @brief
 *
 */
class PixelCentroidTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~PixelCentroidTask() = default;

  virtual void computeProperties(Source& source) const override;


private:

}; /* End of PixelCentroid class */

class PixelCentroidTaskFactory : public TaskFactory {
public:
  PixelCentroidTaskFactory();

  /**
   * @brief Destructor
   */
  virtual ~PixelCentroidTaskFactory() = default;

  virtual std::shared_ptr<Task> getTask(PropertyId property_id) override;
  virtual const std::vector<PropertyId> getProducedProperties() override;

private:
  std::shared_ptr<PixelCentroidTask> m_pixel_centroid_task;
};


} /* namespace SEImplementation */


#endif
