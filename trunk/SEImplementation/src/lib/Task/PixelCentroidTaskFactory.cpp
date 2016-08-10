/**
 * @file src/lib/Task/PixelCentroidTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEImplementation/Property/PixelCentroid.h"
#include "SEImplementation/Task/PixelCentroidTask.h"
#include "SEImplementation/Task/PixelCentroidTaskFactory.h"

namespace SExtractor {

PixelCentroidTaskFactory::PixelCentroidTaskFactory() : m_pixel_centroid_task(std::make_shared<PixelCentroidTask>()) {
}

std::shared_ptr<Task> PixelCentroidTaskFactory::getTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<PixelCentroid>()) {
    return m_pixel_centroid_task;
  } else {
    return nullptr;
  }
}


} // SEImplementation namespace

