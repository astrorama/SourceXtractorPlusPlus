/*  
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General  
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to  
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */

/**
 * @file SourceFlagsSourceTask.h
 *
 * @date Jul 13, 2018
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ATURATEFLAGSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_ATURATEFLAGSOURCETASK_H_

#include <SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h>
#include <SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h>
#include "SEImplementation/Plugin/SaturateFlag/SaturateFlag.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

namespace SExtractor {

class SaturateFlagSourceTask : public SourceTask {
public:
  SaturateFlagSourceTask(unsigned instance): m_instance{instance} {}

  virtual ~SaturateFlagSourceTask() = default;

  virtual void computeProperties(SourceInterface &source) const {
    bool saturate_flag = false;

    const auto measurement_frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
    const auto saturation = measurement_frame->getSaturation();
    const auto measurement_rectangle = source.getProperty<MeasurementFrameRectangle>(m_instance);

    // check whether a saturation value is set
    if (saturation > 0) {
      // iterate over all pixel values
      auto image = measurement_frame->getOriginalImage();
      auto stamp = image->getChunk(
        measurement_rectangle.getTopLeft().m_x, measurement_rectangle.getTopLeft().m_y,
        measurement_rectangle.getWidth(), measurement_rectangle.getHeight());

      for (int y = 0; y < stamp->getHeight(); ++y) {
        for (int x = 0; x < stamp->getWidth(); ++x) {
          if (stamp->getValue(x, y) >= saturation) {
            saturate_flag = true;
            break;
          }
        }
      }
    }

    // set the property
    source.setIndexedProperty<SaturateFlag>(m_instance, saturate_flag);
  };
private:
  unsigned m_instance;

}; // End of SaturateFlagSourceTask class

} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_ATURATEFLAGSOURCETASK_H_ */



