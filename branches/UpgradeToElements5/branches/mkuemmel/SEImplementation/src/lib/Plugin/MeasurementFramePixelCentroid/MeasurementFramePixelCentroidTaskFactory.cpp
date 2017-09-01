/*
 * MeasurementFramePixelCentroidTaskFactory.cpp
 *
 *  Created on: Oct 31, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroidTask.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroidTaskFactory.h"

namespace SExtractor {

std::shared_ptr<Task> MeasurementFramePixelCentroidTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id.getTypeId() == PropertyId::create<MeasurementFramePixelCentroid>().getTypeId()) {
    return std::make_shared<MeasurementFramePixelCentroidTask>(property_id.getIndex());
  } else {
    return nullptr;
  }
}

}
