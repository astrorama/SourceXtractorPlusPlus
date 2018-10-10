/*
 * ApertureFlagTask.cpp
 *
 *  Created on: Oct 09, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <iostream>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfo.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlagTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Plugin/SourceFlags/SourceFlags.h"

namespace SExtractor {

namespace {
const SeFloat CROWD_THRESHOLD_APER = 0.1;
const SeFloat BADAREA_THRESHOLD_APER = 0.1;
}

void ApertureFlagTask::computeProperties(SourceInterface &source) const {
  // get the detection frame
  const auto &detection_frame = source.getProperty<DetectionFrame>().getFrame();

  // get the images and image information from the frame
  const auto &detection_image = detection_frame->getSubtractedImage();
  const auto &detection_variance = detection_frame->getVarianceMap();
  const auto &variance_threshold = detection_frame->getVarianceThreshold();
  const auto &threshold_image = detection_frame->getThresholdedImage();

  // get the object center
  const auto &centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto &centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

  // get the pixel list
  const auto &pix_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  // get the aperture borders on the image
  auto min_pixel = m_aperture->getMinPixel(centroid_x, centroid_y);
  auto max_pixel = m_aperture->getMaxPixel(centroid_x, centroid_y);

  // get the neighbourhood information
  auto neighbour_info = source.getProperty<NeighbourInfo>();

  long int total_flag = 0;
  SeFloat total_area = 0.0;
  SeFloat bad_area = 0;
  SeFloat full_area = 0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {

      // get the area coverage and continue if there is overlap
      auto area = m_aperture->getArea(centroid_x, centroid_y, pixel_x, pixel_y);
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
          total_flag |= SourceFlags::BOUNDARY;
        }
      }
    }
  }

  if (total_area > 0) {
    // check/set the bad area flag
    if (bad_area / total_area > BADAREA_THRESHOLD_APER)
      total_flag |= SourceFlags::BIASED;

    // check/set the crowded area flag
    if (full_area / total_area > CROWD_THRESHOLD_APER)
      total_flag |= SourceFlags::BLENDED;
  }

  // set the source properties
  source.setIndexedProperty<ApertureFlag>(m_instance, total_flag);

  // Draw the checkimage for the aperture on the detection image
  auto aperture_check_img = CheckImages::getInstance().getApertureImage();
  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();

    for (int y = min_pixel.m_y; y <= max_pixel.m_y; ++y) {
      for (int x = min_pixel.m_x; x <= max_pixel.m_x; ++x) {
        if (m_aperture->getArea(centroid_x, centroid_y, x, y) > 0) {
          if (x >= 0 && y >= 0 && x < aperture_check_img->getWidth() && y < aperture_check_img->getHeight()) {
            aperture_check_img->setValue(x, y, src_id);
          }
        }
      }
    }
  }
}

}

