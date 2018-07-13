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

#ifndef _SEIMPLEMENTATION_PLUGIN_BOUNDARYFLAGSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_BOUNDARYFLAGSOURCETASK_H_

#include "SEImplementation/Plugin/BoundaryFlag/BoundaryFlag.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

namespace SExtractor {
class BoundaryFlagSourceTask : public SourceTask {
public:
  virtual ~BoundaryFlagSourceTask() = default;
  virtual void computeProperties(SourceInterface& source) const {
    long int boundary_flag(0);

    // get the image dimensions
    auto x_border = source.getProperty<DetectionFrame>().getFrame()->getOriginalImage()->getWidth() - 1;
    auto y_border = source.getProperty<DetectionFrame>().getFrame()->getOriginalImage()->getHeight() -1;

    // iterate over all pixel coordinates
    for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
      // if a pixel is at the boundary, set the flag and break
      if (pixel_coord.m_x == 0 || pixel_coord.m_x == x_border || pixel_coord.m_y == 0 || pixel_coord.m_y == y_border){
        boundary_flag = 1;
        break;
      }
    }
    source.setProperty<BoundaryFlag>(boundary_flag);
  };
private:
}; // End of BoundaryFlagSourceTask class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_BOUNDARYFLAGSOURCETASK_H_ */



