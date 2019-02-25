/*
 * KronRadiusTask.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: mkuemmel@usm.lmu.de
 */
#include <math.h>
#include <iostream>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/Aperture/EllipticalAperture.h"
#include "SEFramework/Aperture/NeighbourInfo.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusTask.h"

namespace SExtractor {

namespace {
  // the typical radius for determining the Kron-radius
  const SeFloat KRON_NRADIUS           = 3*2.0;
  const SeFloat CROWD_THRESHOLD_KRON   = 0.1;
  const SeFloat BADAREA_THRESHOLD_KRON = 0.1;
}

//////////////////////////////////////////////////////////////////////////////////////////

void KronRadiusTask::computeProperties(SourceInterface& source) const {
  // get the detection frame
  const auto& detection_frame  = source.getProperty<DetectionFrame>().getFrame();

  // get the images and image information from the frame
  const auto& detection_image    = detection_frame->getSubtractedImage();
  const auto& detection_variance = detection_frame->getVarianceMap();
  const auto& variance_threshold = detection_frame->getVarianceThreshold();
  const auto& threshold_image    = detection_frame->getThresholdedImage();

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

  SeFloat radius_flux_sum=0.;
  SeFloat flux_sum=0.;
  long int area_sum=0;
  long int area_bad=0;
  long int area_full=0;
  long int flag=0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat value = 0;
      SeFloat pixel_variance = 0;

      // check whether the current pixel is inside
      if (ell_aper->getArea(centroid_x, centroid_y, pixel_x, pixel_y) > 0){

        // make sure the pixel is inside the image
        if (pixel_x >=0 && pixel_y >=0 && pixel_x < detection_image->getWidth() && pixel_y < detection_image->getHeight()) {

          // enhance the area
          area_sum += 1;

          // get the variance value
          pixel_variance = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;

          // check whether the pixel is good
          if (pixel_variance < variance_threshold) {
            value = detection_image->getValue(pixel_x, pixel_y);
          }
          else {
            // set bad pixel values to zero
            value=0.0;
            pixel_variance=0.0;
            area_bad += 1;
          }

          // check whether the pixel is part of another object
          if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y)) {
            area_full += 1;
          }
          else {
            // add the pixel quantity
            radius_flux_sum += value*sqrt(ell_aper->getRadiusSquared(centroid_x, centroid_y, pixel_x, pixel_y));
            flux_sum        += value;
          }
        }
        else {
          // set the border flag
          flag |= 0x0008;
        }
      }
    }
  }

  if (area_sum>0) {
    // check/set the bad area flag
    if ((SeFloat)area_bad/(SeFloat)area_sum > BADAREA_THRESHOLD_KRON)
      flag |= 0x0001;

    // check/set the crowded area flag
    if ((SeFloat)area_full/(SeFloat)area_sum > CROWD_THRESHOLD_KRON)
      flag |= 0x0002;
  }

  // set the property
  if (flux_sum>0){
    source.setProperty<KronRadius>(radius_flux_sum/flux_sum, flag);
  }
  else {
    // the default value
    source.setProperty<KronRadius>(0.0, flag);
  }
}
}

