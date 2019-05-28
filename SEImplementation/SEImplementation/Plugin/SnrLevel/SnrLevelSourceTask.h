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
 * @file SnrLevelSourceTask.h
 *
 * @date May 27, 2019
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SNRLEVELSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SNRLEVELSOURCETASK_H_

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/SnrLevel/SnrLevel.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

namespace SExtractor {

namespace {
  // the typical radius for determining the Kron-radius
  const SeFloat SNR_MIN_LEVEL = 3.0;
}

class SnrLevelSourceTask : public SourceTask {
public:
  virtual ~SnrLevelSourceTask() = default;
  virtual void computeProperties(SourceInterface& source) const {
    long int n_snr_level=0;

    // get the detection frame and the SNR image
    const auto& detection_frame  = source.getProperty<DetectionFrame>().getFrame();
    const auto& snr_image    = detection_frame->getSnrImage();

    //std::cout << "width: " << snr_image->getWidth() << " height: " << snr_image->getHeight() <<std::endl;

    // go over all pixels
    for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
      auto act_value = snr_image->getValue(pixel_coord.m_x, pixel_coord.m_y);

      if (act_value >= SNR_MIN_LEVEL)
        n_snr_level += 1;
    }
    source.setProperty<SnrLevel>(n_snr_level);
};
private:
}; // End of SnrLevelSourceTask class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_SNRLEVELSOURCETASK_H_ */



