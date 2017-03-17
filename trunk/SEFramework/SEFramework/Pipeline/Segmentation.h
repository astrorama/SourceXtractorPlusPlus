/**
 * @file SEFramework/Pipeline/Segmentation.h
 * @date 05/27/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_SEGMENTATION_H
#define _SEFRAMEWORK_PIPELINE_SEGMENTATION_H

#include <memory>
#include <type_traits>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/Frame/Frame.h"

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageProcessing.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"


namespace SExtractor {

/**
 * @class Segmentation
 * @brief Segmentation takes an image and splits it into individual Sources for further refinement. Each Source
 * results in a notification of the Segmentation's Observers.
 *
 */
class Segmentation : public Observable<std::shared_ptr<SourceInterface>> {

public:
  class Labelling;

  /// Destructor
  virtual ~Segmentation() = default;

  Segmentation(std::shared_ptr<DetectionImageProcessing> image_processing);

  template<class LabellingType, typename ... Args>
  void setLabelling(Args... args) {
    static_assert(std::is_base_of<Labelling, LabellingType>::value,
        "LabellingType must inherit from SExtractor::Segmentation::Labelling");
    static_assert(std::is_constructible<LabellingType, Segmentation&, Args...>::value,
        "LabellingType must be constructible from args");

    m_labelling.reset(new LabellingType(*this, std::forward<Args>(args)...));
  }

  /// Processes a Frame notifying Observers with a Source object for each detection
  void processFrame(std::shared_ptr<DetectionImageFrame> frame);

protected:
  void publishSource(std::shared_ptr<SourceInterface> source) const {
    source->setProperty<DetectionFrame>(m_detection_frame);
    notifyObservers(source);
  }

private:
  std::shared_ptr<DetectionImageFrame> m_detection_frame; // FIXME

  std::unique_ptr<Labelling> m_labelling;
  std::shared_ptr<DetectionImageProcessing> m_filter_image_processing;

}; /* End of Segmentation class */

class Segmentation::Labelling {
public:
  virtual ~Labelling() = default;

  Labelling(const Segmentation& segmentation) : m_segmentation(segmentation) {
  }

  virtual void labelImage(std::shared_ptr<const DetectionImageFrame> frame) = 0;

protected:
  void publishSource(std::shared_ptr<SourceInterface> source) const {
    m_segmentation.publishSource(source);
  }

private:
  const Segmentation& m_segmentation;
};

} /* namespace SExtractor */

#endif
