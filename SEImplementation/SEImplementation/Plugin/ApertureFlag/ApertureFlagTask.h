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
 * @file ApertureFlagSourceTask.h
 *
 * @date Jul 17, 2018
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREFLAGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREFLAGTASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/ApertureFlag/ApertureFlag.h"

namespace SExtractor {
class ApertureFlagTask : public SourceTask {
public:
  ApertureFlagTask(SeFloat aperture_max): m_aperture_max(aperture_max) {};
  virtual ~ApertureFlagTask() = default;
  virtual void computeProperties(SourceInterface& source) const {

    long int aperture_flag=0;

    // check whether something needs to be done
    if (m_aperture_max>0.0){

      // get the width and height of the detection image
      const auto& detection_frame = source.getProperty<DetectionFrame>().getFrame();
      const auto& detection_image = detection_frame->getSubtractedImage();
      const auto& width  = detection_image->getWidth();
      const auto& height = detection_image->getHeight();

      // get the source center
      const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
      const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

      // get the borders of the aperture on the image
      auto max_aperture = std::make_shared<CircularAperture>(m_aperture_max);
      auto min_pixel = max_aperture->getMinPixel(centroid_x, centroid_y);
      auto max_pixel = max_aperture->getMaxPixel(centroid_x, centroid_y);

      // set the flag if the aperture borders are outside of the image
      if (min_pixel.m_x < 0 || min_pixel.m_y < 0 || max_pixel.m_x >= width || max_pixel.m_y >= height)
        aperture_flag = 1;
    }

    // set the flag
    source.setProperty<ApertureFlag>(aperture_flag);
  };
private:
  SeFloat m_aperture_max;
}; // End of ApertureFlagTask class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREFLAGTASK_H_ */



