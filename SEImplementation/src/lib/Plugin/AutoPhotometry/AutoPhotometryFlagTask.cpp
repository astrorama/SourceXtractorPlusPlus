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
 * AutoPhotometryFlagTask.cpp
 *
 *  Created on: Oct 10, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Aperture/FluxMeasurement.h"
#include "SEFramework/Aperture/EllipticalAperture.h"
#include "SEFramework/Aperture/Flagging.h"

#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Plugin/DetectionFrameInfo/DetectionFrameInfo.h"
#include "SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h"

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlag.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlagTask.h"

namespace SourceXtractor {

//////////////////////////////////////////////////////////////////////////////////////////

void AutoPhotometryFlagTask::computeProperties(SourceInterface& source) const {
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

  // get the shape parameters
  const auto& cxx = source.getProperty<ShapeParameters>().getEllipseCxx();
  const auto& cyy = source.getProperty<ShapeParameters>().getEllipseCyy();
  const auto& cxy = source.getProperty<ShapeParameters>().getEllipseCxy();

  // get the pixel list
  const auto& pix_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  // get the kron-radius
  SeFloat kron_radius_auto = m_kron_factor * source.getProperty<KronRadius>().getKronRadius();
  if (kron_radius_auto < m_kron_minrad)
    kron_radius_auto = m_kron_minrad;

  // create the elliptical aperture
  auto ell_aper = std::make_shared<EllipticalAperture>(cxx, cyy, cxy, kron_radius_auto);

  // get the neighbourhood information
  Flags global_flag = computeFlags(ell_aper, centroid_x, centroid_y, pix_list, detection_image,
                                   detection_variance, threshold_image, variance_threshold);

  // set the source properties
  source.setProperty<AutoPhotometryFlag>(global_flag);

  // Draw the aperture
  auto aperture_check_img = CheckImages::getInstance().getAutoApertureImage();
  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();
    fillAperture<int>(ell_aper, centroid_x, centroid_y, aperture_check_img, src_id);
  }
}

}
