/**
 * @file SEImplementation/DetectionFrameSourceStamp.h
 * @date 06/03/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAMESOURCESTAMP_H
#define _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAMESOURCESTAMP_H

#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Task/TaskFactory.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class DetectionFrameSourceStamp
 * @brief
 *
 */
class DetectionFrameSourceStamp : public Property {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStamp() = default;

  DetectionFrameSourceStamp(std::shared_ptr<DetectionImage> stamp) : m_stamp(stamp) {}

  const DetectionImage& getStamp() const {
    return *m_stamp;
  }

private:

  std::shared_ptr<DetectionImage> m_stamp;

}; /* End of DetectionFrameSourceStamp class */


/**
 * @class DetectionFrameSourceStampTask
 * @brief
 *
 */
class DetectionFrameSourceStampTask : public SourceTask {
public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStampTask() = default;

  DetectionFrameSourceStampTask(std::shared_ptr<DetectionImage> image) : m_image(image) {}

  virtual void computeProperties(Source& source) const override;

private:
  std::shared_ptr<DetectionImage> m_image;

}; /* End of DetectionFrameSourceStampTask class */



class DetectionFrameSourceStampTaskFactory : public TaskFactory {
public:
  DetectionFrameSourceStampTaskFactory();

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStampTaskFactory() = default;

  virtual std::shared_ptr<Task> getTask(const PropertyId& property_id) override;
  virtual const std::vector<PropertyId> getProducedProperties() override;

private:
  std::shared_ptr<DetectionFrameSourceStampTask> m_detection_frame_source_stamp_task;
};


} /* namespace SEImplementation */


#endif
