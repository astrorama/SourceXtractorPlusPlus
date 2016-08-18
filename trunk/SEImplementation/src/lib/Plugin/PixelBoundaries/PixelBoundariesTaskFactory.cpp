/**
 * @file src/lib/Task/PixelBoundariesTaskFactory.cpp
 * @date 06/16/16
 * @author mschefer
 */

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTask.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> PixelBoundariesTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<PixelBoundaries>()) {
    return std::make_shared<PixelBoundariesTask>();
  } else {
    return nullptr;
  }
}

} // SEImplementation namespace

