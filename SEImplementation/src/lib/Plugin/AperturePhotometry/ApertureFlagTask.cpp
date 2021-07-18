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
 * ApertureFlagTask.cpp
 *
 *  Created on: Oct 09, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Aperture/FluxMeasurement.h"
#include "SEFramework/Aperture/CircularAperture.h"
#include "SEFramework/Aperture/Flagging.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Plugin/SourceFlags/SourceFlags.h"
#include "SEImplementation/Plugin/DetectionFrameInfo/DetectionFrameInfo.h"
#include "SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h"

#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlagTask.h"

namespace SourceXtractor {


void ApertureFlagTask::computeProperties(SourceInterface &source) const {
  // get the detection frame info
  const auto& detection_frame_info = source.getProperty<DetectionFrameInfo>();
  const auto variance_threshold = detection_frame_info.getVarianceThreshold();

  // get detection frame images
  const auto& detection_frame_images = source.getProperty<DetectionFrameImages>();

  const auto detection_image = detection_frame_images.getLockedImage(LayerSubtractedImage);
  const auto detection_variance = detection_frame_images.getLockedImage(LayerVarianceMap);
  const auto threshold_image = detection_frame_images.getLockedImage(LayerThresholdedImage);

  // get the object center
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

  // get the pixel list
  const auto& pix_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  std::map<float, Flags> all_flags;

  for (auto aperture_diameter : m_apertures) {
    auto aperture = std::make_shared<CircularAperture>(aperture_diameter / 2.);
    auto flag = computeFlags(aperture, centroid_x, centroid_y, pix_list, detection_image,
                             detection_variance, threshold_image, variance_threshold);
    all_flags.emplace(std::make_pair(aperture_diameter, flag));
  }

  // set the source properties
  source.setProperty<ApertureFlag>(all_flags);

  // draw check image for all apertures
  auto aperture_check_img = CheckImages::getInstance().getApertureImage();
  if (aperture_check_img) {
	  for (auto aperture_diameter : m_apertures) {
		  unsigned int src_id = source.getProperty<SourceID>().getId();
		  auto aperture = std::make_shared<CircularAperture>(aperture_diameter / 2.);
		  fillAperture<int>(aperture, centroid_x, centroid_y, aperture_check_img, static_cast<unsigned>(src_id));
	  }
  }
}

} // end of namespace SourceXtractor
