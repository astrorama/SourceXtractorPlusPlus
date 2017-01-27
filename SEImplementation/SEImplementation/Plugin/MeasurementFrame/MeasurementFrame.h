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
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

class MeasurementFrame : public Property {
public:
  MeasurementFrame(std::shared_ptr<MeasurementImage> measurement_image,
      std::shared_ptr<CoordinateSystem> coordinate_system)
      : m_measurement_image(measurement_image),
        m_coordinate_system(coordinate_system),
        m_weight_image(new ConstantImage<WeightImage::PixelType>(measurement_image->getWidth(),
            measurement_image->getHeight(), 1)) {}

  std::shared_ptr<MeasurementImage> getMeasurementImage() const {
    return m_measurement_image;
  }

  std::shared_ptr<MeasurementImage> getWeightImage() const {
    return m_weight_image;
  }

  std::shared_ptr<CoordinateSystem> getCoordinateSystem() const {
    return m_coordinate_system;
  }

private:
  std::shared_ptr<MeasurementImage> m_measurement_image;
  std::shared_ptr<CoordinateSystem> m_coordinate_system;
  std::shared_ptr<WeightImage> m_weight_image;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAME_H_ */



