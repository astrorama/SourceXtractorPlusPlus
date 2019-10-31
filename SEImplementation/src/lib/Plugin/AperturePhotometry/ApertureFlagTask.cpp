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

#include <iostream>

#include "SEFramework/Aperture/CircularAperture.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/Aperture/NeighbourInfo.h"
#include "SEFramework/Source/SourceFlags.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlagTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/SourceFlags/SourceFlags.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

namespace SourceXtractor {

namespace {
const SeFloat CROWD_THRESHOLD_APER = 0.1;
const SeFloat BADAREA_THRESHOLD_APER = 0.1;
}

void ApertureFlagTask::computeProperties(SourceInterface &source) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  // get the detection frame
  const auto& detection_frame = source.getProperty<DetectionFrame>().getFrame();

  // get the images and image information from the frame
  const auto& detection_image = detection_frame->getSubtractedImage();
  const auto& detection_variance = detection_frame->getVarianceMap();
  const auto& variance_threshold = detection_frame->getVarianceThreshold();
  const auto& threshold_image = detection_frame->getThresholdedImage();

  // get the object center
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

  // get the pixel list
  const auto& pix_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  std::map<float, Flags> all_flags;

  for (auto aperture_diameter : m_apertures) {
    auto aperture = CircularAperture(aperture_diameter / 2.);

    // get the aperture borders on the image
    auto min_pixel = aperture.getMinPixel(centroid_x, centroid_y);
    auto max_pixel = aperture.getMaxPixel(centroid_x, centroid_y);

    // get the neighbourhood information
    NeighbourInfo neighbour_info(min_pixel, max_pixel, pix_list, threshold_image);

    Flags flag = Flags::NONE;
    SeFloat total_area = 0.0;
    SeFloat bad_area = 0;
    SeFloat full_area = 0;

    // iterate over the aperture pixels
    for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
      for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {

        // get the area coverage and continue if there is overlap
        auto area = aperture.getArea(centroid_x, centroid_y, pixel_x, pixel_y);
        if (area > 0) {

          // make sure the pixel is inside the image
          if (pixel_x >= 0 && pixel_y >= 0 && pixel_x < detection_image->getWidth() &&
              pixel_y < detection_image->getHeight()) {

            // enhance the area
            total_area += area;

            auto variance_tmp = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;
            if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y) || variance_tmp > variance_threshold) {

              if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y))
                full_area += 1;
              if (variance_tmp > variance_threshold)
                bad_area += 1;
            }
          } else {
            flag |= Flags::BOUNDARY;
          }
        }
      }
    }

    if (total_area > 0) {
      // check/set the bad area flag
      if (bad_area / total_area > BADAREA_THRESHOLD_APER)
        flag |= Flags::BIASED;

      // check/set the crowded area flag
      if (full_area / total_area > CROWD_THRESHOLD_APER)
        flag |= Flags::NEIGHBORS;
    }

    all_flags.emplace(std::make_pair(aperture_diameter, flag));
  }

  // set the source properties
  source.setProperty<ApertureFlag>(all_flags);

  // Draw the checkimage for the last aperture on the detection image
  auto aperture_check_img = CheckImages::getInstance().getApertureImage();
  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();

    auto aperture = CircularAperture(m_apertures[0] / 2.);
    auto min_pixel = aperture.getMinPixel(centroid_x, centroid_y);
    auto max_pixel = aperture.getMaxPixel(centroid_x, centroid_y);

    for (int y = min_pixel.m_y; y <= max_pixel.m_y; ++y) {
      for (int x = min_pixel.m_x; x <= max_pixel.m_x; ++x) {
        if (aperture.getArea(centroid_x, centroid_y, x, y) > 0) {
          if (x >= 0 && y >= 0 && x < aperture_check_img->getWidth() && y < aperture_check_img->getHeight()) {
            aperture_check_img->setValue(x, y, src_id);
          }
        }
      }
    }
  }
}

}

