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
 * DetectionImage.h
 *
 *  Created on: Sep 13, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_
#define _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_

#include "SEFramework/Frame/Frame.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SExtractor {

class DetectionFrame : public Property {
public:
  DetectionFrame(std::shared_ptr<DetectionImageFrame> detection_frame) : m_detection_frame(detection_frame) {
  }

  std::shared_ptr<DetectionImageFrame> getFrame() const {
    return m_detection_frame;
  }

private:
  std::shared_ptr<DetectionImageFrame> m_detection_frame;
};

}

#endif /* _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAME_H_ */
