/**
 * @file src/lib/Segmentation/Lutz.cpp
 * @date 05/31/16
 * @author mschefer
 */

#include "SEImplementation/Segmentation/LutzSegmentation.h"

#include "SEFramework/Image/Image.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/DetectionThreshold.h"


namespace SExtractor {

//
// class LutzSegmentation
//

void LutzSegmentation::labelImage(const DetectionImage& image) {
  Lutz::labelImage(image);
}


void LutzSegmentation::publishGroup(Lutz::PixelGroup& pixel_group) {
  auto source = m_source_factory->createSource();
  source->setProperty<PixelCoordinateList>(pixel_group.pixel_list);
  source->setProperty<DetectionThreshold>(m_detection_threshold);
  publishSource(source);
}



} // Segmentation namespace

