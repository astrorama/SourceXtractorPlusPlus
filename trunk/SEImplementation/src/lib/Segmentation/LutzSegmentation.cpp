/**
 * @file src/lib/Segmentation/Lutz.cpp
 * @date 05/31/16
 * @author mschefer
 */

#include "SEImplementation/Segmentation/LutzSegmentation.h"

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/SubtractImage.h"

#include "SEFramework/Source/SourceWithOnDemandProperties.h"

#include "SEImplementation/Segmentation/Lutz.h"

#include "SEImplementation/Property/PixelCoordinateList.h"

namespace SExtractor {

class LutzLabellingListener : public Lutz::LutzListener {
public:
  LutzLabellingListener(Segmentation::LabellingListener& listener, std::shared_ptr<SourceFactory> source_factory) :
    m_listener(listener),
    m_source_factory(source_factory) {}

  virtual ~LutzLabellingListener() = default;

  void publishGroup(Lutz::PixelGroup& pixel_group) override {
    auto source = m_source_factory->createSource();
    source->setProperty<PixelCoordinateList>(pixel_group.pixel_list);
    m_listener.publishSource(source);
  }

private:
  Segmentation::LabellingListener& m_listener;
  std::shared_ptr<SourceFactory> m_source_factory;
};

//
// class LutzSegmentation
//

void LutzSegmentation::labelImage(Segmentation::LabellingListener& listener, std::shared_ptr<const DetectionImageFrame> frame) {
  Lutz lutz;
  LutzLabellingListener lutz_listener(listener, m_source_factory);
  lutz.labelImage(lutz_listener, *frame->getThresholdedImage());
}

} // Segmentation namespace

