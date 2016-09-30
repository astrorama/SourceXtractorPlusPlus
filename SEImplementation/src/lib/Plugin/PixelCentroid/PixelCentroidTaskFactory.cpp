/**
 * @file src/lib/Task/PixelCentroidTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTask.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> PixelCentroidTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<PixelCentroid>()) {
    return std::make_shared<PixelCentroidTask>();
  } else {
    return nullptr;
  }
}


}

