/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

namespace SourceXtractor {

class MeasurementFrame : public Property {
public:
  MeasurementFrame(std::shared_ptr<MeasurementImageFrame> measurement_frame) : m_measurement_frame(measurement_frame) {
  }

protected:
  std::shared_ptr<MeasurementImageFrame> getFrame() const {
    return m_measurement_frame;
  }

  // To enforce multi-threading safety only those tasks are allowed to use the content of the MeasurementFrame property
  friend class MeasurementFrameCoordinatesTask;
  friend class MeasurementFrameInfoTask;
  friend class MeasurementFrameImagesTask;

private:
  std::shared_ptr<MeasurementImageFrame> m_measurement_frame;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAME_H_ */

