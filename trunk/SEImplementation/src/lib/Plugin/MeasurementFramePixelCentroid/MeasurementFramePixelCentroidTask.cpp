/*
 * MeasurementFramePixelCentroidTask.cpp
 *
 *  Created on: Oct 31, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroidTask.h"

namespace SExtractor {

void MeasurementFramePixelCentroidTask::computeProperties(SourceInterface& source) const {

  // TODO: convert coordinates
  auto pixel_centroid = source.getProperty<PixelCentroid>();
  source.setIndexedProperty<MeasurementFramePixelCentroid>(m_instance, pixel_centroid.getCentroidX(), pixel_centroid.getCentroidY());
}

}


