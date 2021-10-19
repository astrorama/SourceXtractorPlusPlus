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

#ifndef _SEIMPLEMENTATION_PLUGIN_BOUNDARYFLAGSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_BOUNDARYFLAGSOURCETASK_H_

#include "SEImplementation/Plugin/BoundaryFlag/BoundaryFlag.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"

namespace SourceXtractor {

class BoundaryFlagSourceTask : public SourceTask {
public:

  explicit BoundaryFlagSourceTask(unsigned instance): m_instance{instance} {}

  virtual ~BoundaryFlagSourceTask() = default;

  virtual void computeProperties(SourceInterface &source) const {
    bool boundary_flag = false;

    // get the image dimensions
    const auto& measurement_frame_info = source.getProperty<MeasurementFrameInfo>(m_instance);
    auto x_border = measurement_frame_info.getWidth() - 1;
    auto y_border = measurement_frame_info.getHeight() - 1;

    // iterate over all pixel coordinates
    auto measurement_rectangle = source.getProperty<MeasurementFrameRectangle>(m_instance);

    if (measurement_rectangle.getHeight() && measurement_rectangle.getWidth()) {
      auto top_left = measurement_rectangle.getTopLeft();
      auto bottom_right = measurement_rectangle.getBottomRight();

      if (top_left.m_x <= 0 || top_left.m_y <= 0 || bottom_right.m_x >= x_border || bottom_right.m_y >= y_border) {
          boundary_flag = true;
      }
    }

    source.setIndexedProperty<BoundaryFlag>(m_instance, boundary_flag);
  };

private:
  unsigned m_instance;
}; // End of BoundaryFlagSourceTask class

} // namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_BOUNDARYFLAGSOURCETASK_H_ */



