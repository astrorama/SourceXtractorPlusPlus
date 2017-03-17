/**
 * @file src/lib/Segmentation/Lutz.cpp
 * @date 05/31/16
 * @author mschefer
 */

#include "SEImplementation/Segmentation/LutzSegmentation.h"

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/SubtractImage.h"

#include "SEFramework/Source/SourceWithOnDemandProperties.h"

#include "SEImplementation/Property/PixelCoordinateList.h"

namespace SExtractor {

//
// class LutzSegmentation
//

void LutzSegmentation::labelImage(std::shared_ptr<const DetectionImageFrame> frame) {
  Lutz::labelImage(*frame->getThresholdedImage());
}

void LutzSegmentation::publishGroup(Lutz::PixelGroup& pixel_group) {
  auto source = m_source_factory->createSource();
  source->setProperty<PixelCoordinateList>(pixel_group.pixel_list);

  publishSource(source);
}



} // Segmentation namespace

