/*
 * MeasurementFrame.h
 *
 *  Created on: Oct 28, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAME_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAME_H_

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class MeasurementFrame : public Property {
public:
  MeasurementFrame(std::shared_ptr<MeasurementImage> measurement_image) : m_measurement_image(measurement_image) {
  }

  std::shared_ptr<MeasurementImage> getMeasurementImage() const {
    return m_measurement_image;
  }

private:
  std::shared_ptr<MeasurementImage> m_measurement_image;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAME_H_ */



