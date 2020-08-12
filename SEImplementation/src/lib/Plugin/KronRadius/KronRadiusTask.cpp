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
 * KronRadiusTask.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include <math.h>

#include "SEFramework/Aperture/EllipticalAperture.h"
#include "SEFramework/Aperture/NeighbourInfo.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/DetectionFrameInfo/DetectionFrameInfo.h"
#include "SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h"

#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusTask.h"

namespace SourceXtractor {

namespace {
  // the typical radius for determining the Kron-radius
  const SeFloat KRON_NRADIUS           = 3*2.0;
  const SeFloat CROWD_THRESHOLD_KRON   = 0.1;
  const SeFloat BADAREA_THRESHOLD_KRON = 0.1;
}

//////////////////////////////////////////////////////////////////////////////////////////

void KronRadiusTask::computeProperties(SourceInterface& source) const {
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

  // create the elliptical aperture
  auto ell_aper = std::make_shared<EllipticalAperture>(cxx, cyy, cxy, KRON_NRADIUS);

  // get the aperture borders on the image
  const auto& min_pixel = ell_aper->getMinPixel(centroid_x, centroid_y);
  const auto& max_pixel = ell_aper->getMaxPixel(centroid_x, centroid_y);

  // get the pixel list
  const auto& pix_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  // get the neighbourhood information
  NeighbourInfo neighbour_info(min_pixel, max_pixel, pix_list, threshold_image);

  SeFloat radius_flux_sum = 0.;
  SeFloat flux_sum = 0.;
  SeFloat area_sum = 0;
  SeFloat area_bad = 0;
  SeFloat area_full = 0;
  long int flag = 0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {

      // check whether the current pixel is inside
      SeFloat area = ell_aper->getArea(centroid_x, centroid_y, pixel_x, pixel_y);
      if (area <= 0) {
        continue;
      }

      // make sure the pixel is inside the image
      if (pixel_x >= 0 && pixel_y >= 0 && pixel_x < detection_image->getWidth() &&
          pixel_y < detection_image->getHeight()) {
        SeFloat value = 0;

        // enhance the area
        area_sum += 1;

        // get the variance value
        auto pixel_variance = detection_variance->getValue(pixel_x, pixel_y);

        // check whether the pixel is good
        bool is_good = pixel_variance < variance_threshold;
        value = detection_image->getValue(pixel_x, pixel_y) * is_good;
        area_bad += !is_good;

        // check whether the pixel is part of another object
        if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y)) {
          area_full += 1;
        }
        else {
          // add the pixel quantity
          radius_flux_sum += value * sqrt(ell_aper->getRadiusSquared(centroid_x, centroid_y, pixel_x, pixel_y));
          flux_sum += value;
        }
      }
      else {
        // set the border flag
        flag |= 0x0008;
      }
    }
  }

  // check/set the bad area flag
  bool bad_threshold = area_sum > 0 && area_bad / area_sum > BADAREA_THRESHOLD_KRON;
  flag |= 0x0001 * bad_threshold;

  // check/set the crowded area flag
  bool crowded = area_sum > 0 && area_full / area_sum > CROWD_THRESHOLD_KRON;
  flag |= crowded;

  // set the property
  source.setProperty<KronRadius>(flux_sum > 0 ? radius_flux_sum / flux_sum : 0, flag);
}
}

