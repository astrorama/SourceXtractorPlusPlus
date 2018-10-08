/*
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <SEImplementation/CheckImages/CheckImages.h>
#include <SEImplementation/Plugin/SourceIDs/SourceID.h>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfo.h"

namespace SExtractor {

namespace {
  const SeFloat CROWD_THRESHOLD_APER   = 0.1;
  const SeFloat BADAREA_THRESHOLD_APER = 0.1;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryTask::computeProperties(SourceInterface &source) const {
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

  // get the pixel list
  const auto& pix_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  // get the aperture borders on the image
  auto min_pixel = m_aperture->getMinPixel(centroid_x, centroid_y);
  auto max_pixel = m_aperture->getMaxPixel(centroid_x, centroid_y);

  // get the neighbourhood information
  auto neighbour_info = source.getProperty<NeighbourInfo>();

  SeFloat  total_flux     = 0;
  SeFloat  total_variance = 0.0;
  long int total_flag           = 0;
  SeFloat total_area = 0.0;
  SeFloat bad_area=0;
  SeFloat full_area=0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat pixel_value    = 0;
      SeFloat pixel_variance = 0;
      SeFloat variance_tmp   = 0;

      // get the area coverage and continue if there is overlap
      auto area = m_aperture->getArea(centroid_x, centroid_y, pixel_x, pixel_y);
      if (area>0){

        // make sure the pixel is inside the image
        if (pixel_x >=0 && pixel_y >=0 && pixel_x < detection_image->getWidth() && pixel_y < detection_image->getHeight()) {

          // enhance the area
          total_area += area;


          variance_tmp = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;
          if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y) || variance_tmp > variance_threshold) {

            // enhance the area affected by a defect
            if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y))
              full_area+=1;
            if (variance_tmp > variance_threshold)
              bad_area+=1;

            // try using the mirror pixel
            if (m_use_symmetry){
              // get the mirror pixel
              auto mirror_x = 2 * centroid_x - pixel_x + 0.49999;
              auto mirror_y = 2 * centroid_y - pixel_y + 0.49999;
              if (mirror_x >=0 && mirror_y >=0 && mirror_x < detection_image->getWidth() && mirror_y < detection_image->getHeight()) {
                variance_tmp = detection_variance ? detection_variance->getValue(mirror_x, mirror_y) : 1;
                if (!neighbour_info.isNeighbourObjectPixel(mirror_x, mirror_y) && variance_tmp < variance_threshold) {
                  // mirror pixel is OK: take the value
                  pixel_value    = detection_image->getValue(mirror_x, mirror_y);
                  pixel_variance = variance_tmp;
                }
              }
            }
          }
          else {
            pixel_value    = detection_image->getValue(pixel_x, pixel_y);
            pixel_variance = variance_tmp;
          }
          total_flux     += pixel_value * area;
          total_variance += pixel_variance * area;
        }
        else{
          total_flag |= 0x0008;
        }
      }
    }
  }

  if (total_area>0){
    // check/set the bad area flag
    if ((SeFloat)bad_area/(SeFloat)total_area > BADAREA_THRESHOLD_APER)
      total_flag |= 0x0001;

    // check/set the crowded area flag
    if ((SeFloat)full_area/(SeFloat)total_area > CROWD_THRESHOLD_APER)
      total_flag |= 0x0002;
  }

  // compute the derived quantities
  auto flux_error = sqrt(total_variance);
  auto mag = total_flux > 0.0 ? -2.5*log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
  auto mag_error = 1.0857 * flux_error / total_flux;

  // set the source properties
  source.setIndexedProperty<AperturePhotometry>(m_instance, total_flux, flux_error, mag, mag_error, total_flag);

  // Draw the aperture
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

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryAggregateTask::computeProperties(SourceInterface &source) const {
  SeFloat flux = 0;
  for (auto instance : m_instances_to_aggregate) {
    auto aperture_photometry = source.getProperty<AperturePhotometry>(instance);
    flux += aperture_photometry.getFlux();
  }
  flux /= m_instances_to_aggregate.size();

  auto mag = flux > 0.0 ? -2.5 * log10(flux) + m_magnitude_zero_point : SeFloat(99.0);
  source.setIndexedProperty<AperturePhotometry>(m_instance, flux, 999999, mag, 999999, 0); // FIXME
}

}

