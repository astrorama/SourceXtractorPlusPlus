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

#include "SEImplementation/Plugin/SaturateFlag/SaturateFlag.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

namespace SExtractor {
class SaturateFlagSourceTask : public SourceTask {
public:
  virtual ~SaturateFlagSourceTask() = default;
  virtual void computeProperties(SourceInterface& source) const {
    long int saturate_flag(0);

    // get the saturation value
    const auto& saturation      = source.getProperty<DetectionFrame>().getFrame()->getSaturation();

    // check whether a saturation value is set
    if (saturation > 0){
      // iterate over all pixel values
      for (auto value : source.getProperty<DetectionFramePixelValues>().getValues()) {
        // mark a saturated pixel and exit
        if (value > saturation){
          saturate_flag = 1;
          break;
        }
      }
    }
    // set the property
    source.setProperty<SaturateFlag>(saturate_flag);
  };
private:
}; // End of SaturateFlagSourceTask class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_ATURATEFLAGSOURCETASK_H_ */



