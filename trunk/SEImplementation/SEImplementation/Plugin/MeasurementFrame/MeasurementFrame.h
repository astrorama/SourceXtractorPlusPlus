/*
 * MeasurementFrame.h
 *
 *  Created on: Oct 28, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAME_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAME_H_

#include "SEFramework/Frame/Frame.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

class MeasurementFrame : public Property {
public:
  MeasurementFrame(std::shared_ptr<MeasurementImageFrame> measurement_frame) : m_measurement_frame(measurement_frame) {
  }

  std::shared_ptr<Frame<DetectionImage>> getFrame() const {
    return m_measurement_frame;
  }

private:
  std::shared_ptr<MeasurementImageFrame> m_measurement_frame;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAME_H_ */



