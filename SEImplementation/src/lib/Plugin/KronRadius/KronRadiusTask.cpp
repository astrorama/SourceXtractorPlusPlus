/*
 * KronRadiusTask.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: mkuemmel@usm.lmu.de
 */
#include <math.h>
#include <iostream>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTask.h"

#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/KronRadius/KronRadiusTask.h"

namespace SExtractor {

namespace {
  // the typical radius for determining the Kron-radius
  const SeFloat KRON_NRADIUS = 3*2.0;
}

//////////////////////////////////////////////////////////////////////////////////////////

void KronRadiusTask::computeProperties(SourceInterface& source) const {
  // get the detection frame
  const auto& detection_frame  = source.getProperty<DetectionFrame>().getFrame();

  // get the images and image information from the frame
  const auto& detection_image    = detection_frame->getSubtractedImage();
  const auto& detection_variance = detection_frame->getVarianceMap();
  const auto& variance_threshold = detection_frame->getVarianceThreshold();

  // get the object center
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

  // get the shape parameters
  const auto& cxx = source.getProperty<ShapeParameters>().getEllipseCxx();
  const auto& cyy = source.getProperty<ShapeParameters>().getEllipseCyy();
  const auto& cxy = source.getProperty<ShapeParameters>().getEllipseCxy();
  const auto& ell_a = source.getProperty<ShapeParameters>().getEllipseA();
  const auto& ell_b = source.getProperty<ShapeParameters>().getEllipseB();

  // create the elliptical aperture
  auto ell_aper = std::make_shared<EllipticalAperture>(centroid_x, centroid_y, cxx, cyy, cxy, KRON_NRADIUS);

  // get the aperture borders on the image
  const auto& min_pixel = ell_aper->getMinPixel();
  const auto& max_pixel = ell_aper->getMaxPixel();

  SeFloat radius_flux_sum;
  SeFloat flux_sum=0.;
  SeFloat area_sum=0.;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat value = 0;
      SeFloat pixel_variance = 0;

      // make sure the pixel is inside the image
      if (pixel_x >=0 && pixel_y >=0 && pixel_x < detection_image->getWidth() && pixel_y < detection_image->getHeight()) {

        // get the variance value
        pixel_variance = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;

        // check whether the pixel is "good"
        if (pixel_variance < variance_threshold) {
          value = detection_image->getValue(pixel_x, pixel_y);
        } else {
          // set bad pixel values to zero
          value=0.0;
          pixel_variance=0.0;
        }

        // get the area and enhance the total quantities
        auto area           = ell_aper->getArea(pixel_x, pixel_y);
        auto radius_squared = ell_aper->getRadiusSquared(pixel_x, pixel_y);
        radius_flux_sum += area*value*sqrt(radius_squared);
        flux_sum        += area*value;
        area_sum        += area;
      }
    }
  }

  // set the property
  if (area_sum>0)
    source.setProperty<KronRadius>(radius_flux_sum/flux_sum);
  else
    source.setProperty<KronRadius>(0.0);
}
}

