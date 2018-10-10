/*
 * AutoPhotometryFlagTask.cpp
 *
 *  Created on: Oct 10, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Aperture/EllipticalAperture.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfo.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlag.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlagTask.h"
#include "SEImplementation/Plugin/SourceFlags/SourceFlags.h"


namespace SExtractor {

namespace {
const SeFloat CROWD_THRESHOLD_AUTO = 0.1;
const SeFloat BADAREA_THRESHOLD_AUTO = 0.1;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AutoPhotometryFlagTask::computeProperties(SourceInterface &source) const {
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

  // get the shape parameters
  const auto &cxx = source.getProperty<ShapeParameters>().getEllipseCxx();
  const auto &cyy = source.getProperty<ShapeParameters>().getEllipseCyy();
  const auto &cxy = source.getProperty<ShapeParameters>().getEllipseCxy();

  // get the kron-radius
  SeFloat kron_radius_auto = m_kron_factor * source.getProperty<KronRadius>().getKronRadius();
  if (kron_radius_auto < m_kron_minrad)
    kron_radius_auto = m_kron_minrad;

  // create the elliptical aperture
  auto ell_aper = std::make_shared<EllipticalAperture>(cxx, cyy, cxy, kron_radius_auto);

  // get the aperture borders on the image
  const auto &min_pixel = ell_aper->getMinPixel(centroid_x, centroid_y);
  const auto &max_pixel = ell_aper->getMaxPixel(centroid_x, centroid_y);

  // get the neighbourhood information
  auto neighbour_info = source.getProperty<NeighbourInfo>();

  long int area_sum = 0;
  long int area_bad = 0;
  long int area_full = 0;
  long int total_flag = 0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat variance_tmp = 0;

      // check whether the pixel is in the ellipse
      if (ell_aper->getArea(centroid_x, centroid_y, pixel_x, pixel_y) > 0) {

        // check whether the pixel is inside the image
        if (pixel_x >= 0 && pixel_y >= 0 && pixel_x < detection_image->getWidth() &&
            pixel_y < detection_image->getHeight()) {

          // enhance the area
          area_sum += 1;

          // check whether the pixel is OK
          variance_tmp = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;
          if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y) || variance_tmp > variance_threshold) {
            // enhance the area affected by a defect
            if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y))
              area_full += 1;
            if (variance_tmp > variance_threshold)
              area_bad += 1;
          }
        } else {
          // set the border flag
          total_flag |= SourceFlags::BOUNDARY;
        }
      }
    }
  }

  if (area_sum > 0) {
    // check/set the bad area flag
    if ((SeFloat) area_bad / (SeFloat) area_sum > BADAREA_THRESHOLD_AUTO)
      total_flag |= SourceFlags::BIASED;

    // check/set the crowded area flag
    if ((SeFloat) area_full / (SeFloat) area_sum > CROWD_THRESHOLD_AUTO)
      total_flag |= SourceFlags::BLENDED;
  }

  // set the source properties
  source.setProperty<AutoPhotometryFlag>(total_flag);

  // Draw the aperture
  auto aperture_check_img = CheckImages::getInstance().getAutoApertureImage();
  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();

    for (int y = min_pixel.m_y; y <= max_pixel.m_y; ++y) {
      for (int x = min_pixel.m_x; x <= max_pixel.m_x; ++x) {
        if (ell_aper->getArea(centroid_x, centroid_y, x, y) > 0) {
          if (x >= 0 && y >= 0 && x < aperture_check_img->getWidth() && y < aperture_check_img->getHeight()) {
            aperture_check_img->setValue(x, y, src_id);
          }
        }
      }
    }
  }
}

}
