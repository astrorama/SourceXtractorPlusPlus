/*
 * MeasurementFrameTask.cpp
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrameTask.h"

namespace SExtractor {

void MeasurementFrameTask::computeProperties(SourceInterface& source) const {
  source.setIndexedProperty<MeasurementFrame>(m_instance, m_measurement_image);
}

void DefaultMeasurementFrameTask::computeProperties(SourceInterface& source) const {
  auto detection_image = source.getProperty<DetectionFrame>().getDetectionImage();
  source.setIndexedProperty<MeasurementFrame>(m_instance, detection_image);
}

}





