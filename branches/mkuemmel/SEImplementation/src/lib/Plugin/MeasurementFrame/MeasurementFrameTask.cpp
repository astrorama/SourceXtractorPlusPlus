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
  source.setIndexedProperty<MeasurementFrame>(m_instance, m_measurement_frame);
}

void DefaultMeasurementFrameTask::computeProperties(SourceInterface& source) const {
  const auto& detection_frame = source.getProperty<DetectionFrame>();

  source.setIndexedProperty<MeasurementFrame>(
      m_instance,
      detection_frame.getFrame());
}

}





