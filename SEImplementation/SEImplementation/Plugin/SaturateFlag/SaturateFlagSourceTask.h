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

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/DetectionFrame.h"

#include <SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h>
#include <SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h>
#include <SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h>
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

#include "SEImplementation/Plugin/SaturateFlag/SaturateFlag.h"

namespace SourceXtractor {

class SaturateFlagSourceTask : public SourceTask {
public:
  SaturateFlagSourceTask(unsigned instance): m_instance{instance} {}

  virtual ~SaturateFlagSourceTask() = default;

  virtual void computeProperties(SourceInterface &source) const {
    bool saturate_flag = false;

    const auto& measurement_frame_info = source.getProperty<MeasurementFrameInfo>(m_instance);
    const auto& measurement_frame_images = source.getProperty<MeasurementFrameImages>(m_instance);

    const auto saturation = measurement_frame_info.getSaturation();
    const auto measurement_rectangle = source.getProperty<MeasurementFrameRectangle>(m_instance);

    if (saturation > 0 && measurement_rectangle.getWidth()) {
      // iterate over all pixel values
      auto stamp = measurement_frame_images.getImageChunk(LayerOriginalImage,
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

} // namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_ATURATEFLAGSOURCETASK_H_ */



