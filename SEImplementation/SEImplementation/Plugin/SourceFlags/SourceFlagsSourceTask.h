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
 * @date Jul 12, 2018
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGSOURCETASK_H_

#include <iostream>

#include "SEImplementation/Plugin/SourceFlags/SourceFlags.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

namespace SExtractor {
class SourceFlagsSourceTask : public SourceTask {
public:
  virtual ~SourceFlagsSourceTask() = default;
  virtual void computeProperties(SourceInterface& source) const {
    long int source_flag(0);

    auto x_border = source.getProperty<DetectionFrame>().getFrame()->getOriginalImage()->getWidth() - 1;
    auto y_border = source.getProperty<DetectionFrame>().getFrame()->getOriginalImage()->getHeight() -1;
    for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
      if (pixel_coord.m_x == 0 || pixel_coord.m_x == x_border){
        source_flag += 2;
        break;
      }
      if (pixel_coord.m_y == 0 || pixel_coord.m_y == y_border){
        source_flag += 2;
        break;
      }
    }

    const auto& saturation      = source.getProperty<DetectionFrame>().getFrame()->getSaturation();
    for (auto value : source.getProperty<DetectionFramePixelValues>().getValues()) {
      if (saturation > 0. &&  value > saturation)
        source_flag += 1;
      break;
    }

    source.setProperty<SourceFlags>(source_flag);
  };
private:
}; // End of SourceFlagsSourceTask class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGSOURCETASK_H_ */



